#include "Material.h"

#include <fstream>

#include "Graphics.h"
#include "ResourceManagement/ResourceManager.h"
#include "Utility/JSON.h"

static const String DEFAULT_SHADER_PATH = DT_TEXT("Resources/Shaders/Color");

Material::Material() : _perFrameBuffer(nullptr), _shader(nullptr)
{
}

Material::Material(SharedPtr<Material> other) : _perFrameBuffer(nullptr), _shader(nullptr)
{

}

Material::~Material()
{

}

bool Material::Load(const String& path)
{
	Asset::Load(path);

	std::ifstream materialFile(path);
	if(!materialFile.is_open())
	{
		return false;
	}

	JSON materialData = JSON::parse(materialFile);
	String shaderPath = materialData["Shader"];
	_queue = materialData["Queue"];
	CullMode cullMode = EnumInfo<CullMode>::FromString(materialData["Cull"]);
	FillMode fillMode = EnumInfo<FillMode>::FromString(materialData["Fill"]);
	ZWrite zWrite = EnumInfo<ZWrite>::FromString(materialData["ZWrite"]);
	CompareFunction zTest = EnumInfo<CompareFunction>::FromString(materialData["ZTest"]);
	BlendMode srcBlendMode = EnumInfo<BlendMode>::FromString(materialData["SrcBlend"]);
	BlendMode destBlendMode = EnumInfo<BlendMode>::FromString(materialData["DestBlend"]);
	_color = materialData["Color"];

	materialFile.close();

	_shader = GetResourceManager().Get<Shader>(shaderPath);

	_renderStateParams = RenderStateParams(cullMode, fillMode, zWrite, srcBlendMode, destBlendMode, zTest);

	return true;
}

bool Material::Save(const String& path)
{
	std::ofstream materialFile(path);
	if(!materialFile.is_open())
	{
		return false;
	}

	JSON materialData;
	if(_shader)
	{
		materialData["Shader"] = _shader->GetPath();
	}
	else
	{
		materialData["Shader"] = DEFAULT_SHADER_PATH;
	}
	materialData["Queue"] = _queue;
	materialData["Cull"] = EnumInfo<CullMode>::ToString(_renderStateParams.GetCullMode());
	materialData["Fill"] = EnumInfo<FillMode>::ToString(_renderStateParams.GetFillMode());
	materialData["ZWrite"] = EnumInfo<ZWrite>::ToString(_renderStateParams.GetZWrite());
	materialData["ZTest"] = EnumInfo<CompareFunction>::ToString(_renderStateParams.GetZTestFunction());
	materialData["SrcBlend"] = EnumInfo<BlendMode>::ToString(_renderStateParams.GetSrcBlendMode());
	materialData["DestBlend"] = EnumInfo<BlendMode>::ToString(_renderStateParams.GetDestBlendMode());
	materialData["Color"] = _color;

	std::string materialDataString = materialData.dump(1, '\t');
	materialFile.write(materialDataString.c_str(), materialDataString.size());
	materialFile.close();

	return true;
}

bool Material::Initialize()
{
	_queue = OpaqueUpperLimit;
	
	D3D11_SUBRESOURCE_DATA bufferData = { 0 };
	bufferData.pSysMem = &_color;

	D3D11_BUFFER_DESC bufferDesc = { 0 };
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = sizeof(XMFLOAT4);
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	Graphics& graphics = GetGraphics();
	if (!graphics.CreateBuffer(bufferDesc, bufferData, &_perFrameBuffer))
	{
		GetDebug().Print(LogVerbosity::Error, CHANNEL_GRAPHICS, DT_TEXT("Failed to create constant buffer for PerFrame data"));
		return false;
	}

	if(!graphics.CreateRenderState(_renderState, _renderStateParams))
	{
		GetDebug().Print(LogVerbosity::Error, CHANNEL_GRAPHICS, DT_TEXT("Failed to create render state"));
		return false;
	}

	if(!_shader)
	{
		_shader = GetResourceManager().Get<Shader>(DEFAULT_SHADER_PATH);
	}

	return true;
}

void Material::Shutdown()
{
	if(_renderState != nullptr)
	{
		_renderState->Shutdown();
		_renderState = nullptr;
	}
	RELEASE_COM(_perFrameBuffer);
}

void Material::SetPerFrameParameters(Graphics& graphics)
{
	graphics.SetRenderState(_renderState);

	if (_shader)
	{
		_shader->SetPerFrameParameters(graphics);
	}

	XMFLOAT4* data = (XMFLOAT4*)graphics.Map(_perFrameBuffer);
	if (!data)
	{
		graphics.Unmap(_perFrameBuffer);
		return;
	}

	*data = _color;
	graphics.Unmap(_perFrameBuffer);
	graphics.SetVSConstantBuffers(2, 1, &_perFrameBuffer);
}

void Material::SetPerObjectParameters(Graphics& graphics, SharedPtr<GameObject> gameObject)
{
	if (_shader)
	{
		_shader->SetPerObjectParameters(graphics, gameObject);
	}
}

void Material::SetWorldMatrix(Graphics& graphics, const XMMATRIX& worldMatrix)
{
	if(_shader)
	{
		_shader->SetWorldMatrix(graphics, worldMatrix);
	}
}
