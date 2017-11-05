#include "Shader.h"

#include "Graphics.h"
#include "GameFramework/GameObject.h"
#include "GameFramework/Components/Camera.h"

#include <d3d11.h>
#include <d3dcompiler.h>

Shader::Shader() : _vertexShader(nullptr), _pixelShader(nullptr), _inputLayout(nullptr), _perFrameBuffer(nullptr), _perObjectBuffer(nullptr)
{
}

Shader::~Shader()
{

}

bool Shader::Initialize(const string& path)
{
	Asset::Initialize(path);

	const string vsFileName = path + DT_TEXT("VS.hlsl");
	const string psFileName = path + DT_TEXT("PS.hlsl");

	assert(gGraphics);
	ID3D10Blob* vertexShaderBuffer;
	HRESULT result = D3DCompileFromFile(vsFileName.c_str(), nullptr, nullptr, "main", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, nullptr);
	HR(result);

	if(!gGraphics->CreateVertexShader(vertexShaderBuffer, &_vertexShader))
	{
		return false;
	}

	ID3D10Blob* pixelShaderBuffer;
	result = D3DCompileFromFile(psFileName.c_str(), nullptr, nullptr, "main", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, nullptr);
	HR(result);

	if(!gGraphics->CreatePixelShader(pixelShaderBuffer, &_pixelShader))
	{
		return false;
	}

	RELEASE_COM(pixelShaderBuffer);

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

	if(!gGraphics->CreateInputLayout(inputLayoutDesc, sizeof(inputLayoutDesc) / sizeof(D3D11_INPUT_ELEMENT_DESC), vertexShaderBuffer->GetBufferPointer(), (uint64)vertexShaderBuffer->GetBufferSize(), &_inputLayout))
	{
		return false;
	}

	RELEASE_COM(vertexShaderBuffer);

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

	if(!gGraphics->CreateBuffer(bufferDesc, bufferData, &_perFrameBuffer))
	{
		return false;
	}

	bufferDesc.ByteWidth = sizeof(PerObjectBuffer);
	PerObjectBuffer pob;
	pob.world = XMMatrixIdentity();
	bufferData.pSysMem = &pob;

	if(!gGraphics->CreateBuffer(bufferDesc, bufferData, &_perObjectBuffer))
	{
		return false;
	}

	return true;
}

void Shader::Shutdown()
{
	RELEASE_COM(_perObjectBuffer);
	RELEASE_COM(_perFrameBuffer);
	RELEASE_COM(_inputLayout);
	RELEASE_COM(_pixelShader);
	RELEASE_COM(_vertexShader);
}

void Shader::SetPerFrameParameters(ID3D11DeviceContext* deviceContext)
{
	assert(gGraphics);
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT result = deviceContext->Map(_perFrameBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return;
	}

	PerFrameBuffer* data = (PerFrameBuffer*)mappedResource.pData;
	if (!data)
	{
		deviceContext->Unmap(_perFrameBuffer, 0);
		return;
	}
	data->view = XMMatrixTranspose(Camera::GetMainCamera()->GetViewMatrix());
	data->projection = XMMatrixTranspose(Camera::GetMainCamera()->GetProjectionMatrix());
	
	deviceContext->Unmap(_perFrameBuffer, 0);
	deviceContext->VSSetConstantBuffers(0, 1, &_perFrameBuffer);
}

void Shader::SetPerObjectParameters(ID3D11DeviceContext* deviceContext, GameObject* gameObject)
{
	assert(gGraphics);
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT result = deviceContext->Map(_perObjectBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return;
	}

	PerObjectBuffer* data = (PerObjectBuffer*)mappedResource.pData;
	if (!data)
	{
		deviceContext->Unmap(_perObjectBuffer, 0);
		return;
	}
	data->world = XMMatrixTranspose(gameObject->GetTransform().GetModelMatrix());

	deviceContext->Unmap(_perObjectBuffer, 0);
	deviceContext->VSSetConstantBuffers(1, 1, &_perObjectBuffer);
}
