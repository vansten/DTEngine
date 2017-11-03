#pragma once

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include <d3d11.h>

#include "Core/Platform.h"
#include "Utility/Math.h"

class Window;
class MeshBase;
class Material;
class MeshRenderer;
class GameObject;

class Graphics
{
private:
	IDXGISwapChain* _swapChain;
	ID3D11Device* _device;
	ID3D11DeviceContext* _deviceContext;
	ID3D11RenderTargetView* _renderTargetView;
	ID3D11Texture2D* _depthStencilBuffer;
	ID3D11DepthStencilState* _depthStencilState;
	ID3D11DepthStencilView* _depthStencilView;
	ID3D11RasterizerState* _rasterizerState;

	Material* _lastUsedMaterial;
	GameObject* _currentlyRenderedObject;
	bool _vsync;

public:
	Graphics();

private:
	bool GetRefreshRate(uint32 windowHeight, uint32& numerator, uint32& denominator);

public:
	bool Initialize(Window* window, bool vsync);
	void Shutdown();

	void BeginScene(D3D11_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	void EndScene();

	bool CreateBuffer(const D3D11_BUFFER_DESC& bufferDesc, const D3D11_SUBRESOURCE_DATA& bufferData, ID3D11Buffer** bufferPtr);
	bool CreateVertexShader(ID3D10Blob* shaderBuffer, ID3D11VertexShader** vertexShader);
	bool CreatePixelShader(ID3D10Blob* shaderBuffer, ID3D11PixelShader** pixelShader);
	bool CreateInputLayout(D3D11_INPUT_ELEMENT_DESC const* inputLayoutDesc, uint8 inputLayoutDescSize, void* shaderBufferPointer, uint64 shaderBufferSize, ID3D11InputLayout** inputLayout);

	void SetObject(GameObject* gameObject);
	void SetMaterial(Material* material);
	void DrawIndexed(ID3D11Buffer* vertexBuffer, ID3D11Buffer* indexBuffer, uint32 indicesCount, uint32 stride, uint32 offset);
};

extern Graphics* gGraphics;

#define RELEASE_COM(comObject) \
if(comObject) \
{ \
	comObject->Release(); \
	comObject = 0; \
}

#define HR(result) \
if(FAILED(result)) \
{ \
	return false; \
}
