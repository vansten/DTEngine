#include "Shader.h"

#include "Debug/Debug.h"

#include "Graphics.h"
#include "GameFramework/Entity.h"
#include "GameFramework/Components/Camera.h"

#include <d3d11.h>
#include <d3dcompiler.h>
#include <d3d11shader.h>

Shader::Shader() : _vertexShader(nullptr), _pixelShader(nullptr), _inputLayout(nullptr), _perFrameBuffer(nullptr), _perObjectBuffer(nullptr)
{
}

Shader::~Shader()
{

}

bool Shader::GatherConstantBuffersInfo(ID3D10Blob* compiledShader)
{
	if(compiledShader == nullptr)
	{
		return false;
	}

	ID3D11ShaderReflection* reflectedShader = nullptr;
	HRESULT result = D3DReflect(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&reflectedShader);
	HR_REACTION(result, GetDebug().Print(LogVerbosity::Error, CHANNEL_GRAPHICS, DT_TEXT("Cannot reflect shader!")));

	// Get shader description
	D3D11_SHADER_DESC reflectedShaderDesc;
	reflectedShader->GetDesc(&reflectedShaderDesc);

	// For each of bound resource (textures, samplers, constant buffers)
	for(uint32 i = 0; i < reflectedShaderDesc.BoundResources; ++i)
	{
		D3D11_SHADER_INPUT_BIND_DESC reflectedBoundResourceDesc;
		result = reflectedShader->GetResourceBindingDesc(i, &reflectedBoundResourceDesc);

		switch(reflectedBoundResourceDesc.Type)
		{
		case D3D_SIT_CBUFFER:
			{
				ID3D11ShaderReflectionConstantBuffer* reflectedConstantBuffer = reflectedShader->GetConstantBufferByName(reflectedBoundResourceDesc.Name);
				CreateConstantBufferAndVariables(reflectedBoundResourceDesc, reflectedConstantBuffer);
			}
			break;
		case D3D_SIT_TEXTURE:
			break;
		case D3D_SIT_SAMPLER:
			break;
		}
	}

	return true;
}

bool Shader::CreateConstantBufferAndVariables(const _D3D11_SHADER_INPUT_BIND_DESC& reflectedResourceDesc, ID3D11ShaderReflectionConstantBuffer* reflectedConstantBuffer)
{
	D3D11_SHADER_BUFFER_DESC reflectedConstantBufferDesc;
	HRESULT result = reflectedConstantBuffer->GetDesc(&reflectedConstantBufferDesc);
	HR_REACTION(result, GetDebug().Print(LogVerbosity::Error, CHANNEL_GRAPHICS, DT_TEXT("Cannot obtain constant buffer description!")));

	// Create constant buffer
	UniquePtr<ShaderConstantBuffer> constantBuffer = std::make_unique<ShaderConstantBuffer>();
	std::string name = reflectedConstantBufferDesc.Name;
	constantBuffer->Name = String(name.begin(), name.end());
	constantBuffer->Index = reflectedResourceDesc.BindPoint;

	// Create variables that given constant buffer contains
	for(uint32 j = 0; j < reflectedConstantBufferDesc.Variables; ++j)
	{
		// Get variable description
		ID3D11ShaderReflectionVariable* reflectedVariable = reflectedConstantBuffer->GetVariableByIndex(j);
		D3D11_SHADER_VARIABLE_DESC reflectedVariableDesc;
		result = reflectedVariable->GetDesc(&reflectedVariableDesc);
		HR_REACTION(result, GetDebug().Print(LogVerbosity::Error, CHANNEL_GRAPHICS, DT_TEXT("Cannot obtain variable description!")));

		// Create variable
		UniquePtr<ShaderVariable> variable = std::make_unique<ShaderVariable>();
		variable->Offset = reflectedVariableDesc.StartOffset;
		std::string name = reflectedVariableDesc.Name;
		variable->Name = String(name.begin(), name.end());
		variable->Size = reflectedVariableDesc.Size;

		// Push variable to array
		constantBuffer->Variables.push_back(std::move(variable));
	}

	// Push constant buffer to array
	_constantBuffers.push_back(std::move(constantBuffer));

	return true;
}

bool Shader::Load(const String& path)
{
	Asset::Load(path);

	const String vsFileName = path + DT_TEXT("VS.hlsl");
	const String psFileName = path + DT_TEXT("PS.hlsl");
	Graphics& graphics = GetGraphics();

	HRESULT result = D3DCompileFromFile(vsFileName.c_str(), nullptr, nullptr, "main", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &_vertexShaderBuffer, nullptr);
	HR(result);

	result = D3DCompileFromFile(psFileName.c_str(), nullptr, nullptr, "main", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &_pixelShaderBuffer, nullptr);
	HR(result);
	
	return true;
}

bool Shader::Initialize()
{
	if(!_vertexShaderBuffer || !_pixelShaderBuffer)
	{
		return false;
	}

	Graphics& graphics = GetGraphics();
	
	if(!graphics.CreateVertexShader(_vertexShaderBuffer, &_vertexShader))
	{
		GetDebug().Print(LogVerbosity::Error, CHANNEL_GRAPHICS, DT_TEXT("Failed to create vertex shader"));
		return false;
	}
	if(!graphics.CreatePixelShader(_pixelShaderBuffer, &_pixelShader))
	{
		GetDebug().Print(LogVerbosity::Error, CHANNEL_GRAPHICS, DT_TEXT("Failed to create pixel shader"));
		return false;
	}

	if(!GatherConstantBuffersInfo(_vertexShaderBuffer))
	{
		GetDebug().Print(LogVerbosity::Error, CHANNEL_GRAPHICS, DT_TEXT("Failed to obtain shader reflection info"));
		return false;
	}

	if(!GatherConstantBuffersInfo(_pixelShaderBuffer))
	{
		GetDebug().Print(LogVerbosity::Error, CHANNEL_GRAPHICS, DT_TEXT("Failed to obtain shader reflection info"));
		return false;
	}

	RELEASE_COM(_pixelShaderBuffer);

	D3D11_INPUT_ELEMENT_DESC inputLayoutDesc[3];
	inputLayoutDesc[0] = {0};
	inputLayoutDesc[1] = {0};
	inputLayoutDesc[2] = {0};

	inputLayoutDesc[0].SemanticName = "POSITION";
	inputLayoutDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputLayoutDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	inputLayoutDesc[1].SemanticName = "NORMAL";
	inputLayoutDesc[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputLayoutDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	inputLayoutDesc[2].SemanticName = "TEXCOORD";
	inputLayoutDesc[2].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputLayoutDesc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	if(!graphics.CreateInputLayout(inputLayoutDesc, sizeof(inputLayoutDesc) / sizeof(D3D11_INPUT_ELEMENT_DESC), _vertexShaderBuffer->GetBufferPointer(), (uint64)_vertexShaderBuffer->GetBufferSize(), &_inputLayout))
	{
		GetDebug().Print(LogVerbosity::Error, CHANNEL_GRAPHICS, DT_TEXT("Failed to create input layout"));
		return false;
	}

	RELEASE_COM(_vertexShaderBuffer);

	D3D11_SUBRESOURCE_DATA bufferData = {0};
	PerFrameBuffer pfb;
	pfb.view = XMMatrixIdentity();
	pfb.projection = XMMatrixIdentity();
	bufferData.pSysMem = &pfb;

	D3D11_BUFFER_DESC bufferDesc = {0};
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = sizeof(PerFrameBuffer);
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	if(!graphics.CreateBuffer(bufferDesc, bufferData, &_perFrameBuffer))
	{
		GetDebug().Print(LogVerbosity::Error, CHANNEL_GRAPHICS, DT_TEXT("Failed to create constant buffer for PerFrame data"));
		return false;
	}

	bufferDesc.ByteWidth = sizeof(PerObjectBuffer);
	PerObjectBuffer pob;
	pob.world = XMMatrixIdentity();
	bufferData.pSysMem = &pob;

	if(!graphics.CreateBuffer(bufferDesc, bufferData, &_perObjectBuffer))
	{
		GetDebug().Print(LogVerbosity::Error, CHANNEL_GRAPHICS, DT_TEXT("Failed to create constant buffer for PerObject data"));
		return false;
	}

	return true;
}

void Shader::Shutdown()
{
	_constantBuffers.clear();
	RELEASE_COM(_perObjectBuffer);
	RELEASE_COM(_perFrameBuffer);
	RELEASE_COM(_inputLayout);
	RELEASE_COM(_pixelShader);
	RELEASE_COM(_vertexShader);
	RELEASE_COM(_vertexShaderBuffer);
	RELEASE_COM(_pixelShaderBuffer);
}

void Shader::SetPerFrameParameters(Graphics& graphics)
{
	PerFrameBuffer* data = (PerFrameBuffer*)graphics.Map(_perFrameBuffer);
	if (!data)
	{
		graphics.Unmap(_perFrameBuffer);
		return;
	}
	data->view = XMMatrixTranspose(Camera::GetMainCamera()->GetViewMatrix());
	data->projection = XMMatrixTranspose(Camera::GetMainCamera()->GetProjectionMatrix());
	
	graphics.Unmap(_perFrameBuffer);
	graphics.SetVSConstantBuffers(0, 1, &_perFrameBuffer);
}

void Shader::SetPerObjectParameters(Graphics& graphics, Entity* entity)
{
	PerObjectBuffer* data = (PerObjectBuffer*)graphics.Map(_perObjectBuffer);
	if (!data)
	{
		graphics.Unmap(_perObjectBuffer);
		return;
	}
	data->world = XMMatrixTranspose(entity->GetTransform().GetModelMatrix());

	graphics.Unmap(_perObjectBuffer);
	graphics.SetVSConstantBuffers(1, 1, &_perObjectBuffer);
}

void Shader::SetWorldMatrix(Graphics& graphics, const XMMATRIX& worldMatrix)
{
	PerObjectBuffer* data = (PerObjectBuffer*)graphics.Map(_perObjectBuffer);
	if(!data)
	{
		graphics.Unmap(_perObjectBuffer);
		return;
	}
	data->world = XMMatrixTranspose(worldMatrix);

	graphics.Unmap(_perObjectBuffer);
	graphics.SetVSConstantBuffers(1, 1, &_perObjectBuffer);
}
