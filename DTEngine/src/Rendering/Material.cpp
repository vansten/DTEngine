#include "Material.h"

#include "Graphics.h"
#include "ResourceManagement/ResourceManager.h"

Material::Material() : _perFrameBuffer(nullptr), _shader(nullptr)
{
}

Material::~Material()
{
}

bool Material::Initialize(const String& path)
{
	Asset::Initialize(path);

	ResourceManager& resourceManager = GetResourceManager();

	// Basic shader
	_shader = resourceManager.Load<Shader>(COLOR_SHADER);
	_color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	
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
		return false;
	}

	return true;
}

void Material::Shutdown()
{
	RELEASE_COM(_perFrameBuffer);
}

void Material::SetPerFrameParameters(Graphics& graphics)
{
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
