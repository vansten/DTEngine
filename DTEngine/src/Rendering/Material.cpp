#include "Material.h"

#include "Graphics.h"
#include "ResourceManagement/ResourceManager.h"

Material::Material()
{
}

Material::~Material()
{
}

bool Material::Initialize(const string& path)
{
	Asset::Initialize(path);

	assert(gResourceManager);

	_shader = gResourceManager->Load<Shader>(path);
	_color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	assert(gGraphics);

	D3D11_SUBRESOURCE_DATA bufferData = { 0 };
	bufferData.pSysMem = &_color;

	D3D11_BUFFER_DESC bufferDesc = { 0 };
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = sizeof(XMFLOAT4);
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	if (!gGraphics->CreateBuffer(bufferDesc, bufferData, &_perFrameBuffer))
	{
		return false;
	}

	return true;
}

void Material::Shutdown()
{
	RELEASE_COM(_perFrameBuffer);
}

void Material::SetPerFrameParameters(ID3D11DeviceContext* deviceContext)
{
	if (_shader)
	{
		_shader->SetPerFrameParameters(deviceContext);
	}

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT result = deviceContext->Map(_perFrameBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return;
	}

	XMFLOAT4* data = (XMFLOAT4*)mappedResource.pData;
	if (!data)
	{
		deviceContext->Unmap(_perFrameBuffer, 0);
		return;
	}

	*data = _color;
	deviceContext->Unmap(_perFrameBuffer, 0);
	deviceContext->VSSetConstantBuffers(2, 1, &_perFrameBuffer);
}

void Material::SetPerObjectParameters(ID3D11DeviceContext* deviceContext, GameObject* gameObject)
{
	if (_shader)
	{
		_shader->SetPerObjectParameters(deviceContext, gameObject);
	}
}
