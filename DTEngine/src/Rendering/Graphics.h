#pragma once

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include <d3d11.h>

#include "Core/App.h"
#include "Core/Platform.h"
#include "Utility/Math.h"
#include "Utility/EnumUtility.h"
#include "RenderState.h"

class Window;
class MeshBase;
class Material;
class MeshRenderer;
class GameObject;

class Graphics
{
public:
	class CommonRenderStates
	{
		friend class Graphics;

	public:
		static RenderState WireframeRenderState;
		static RenderState DefaultRenderState;

	private:
		static bool Initialize(ID3D11Device* device);
		static void Shutdown();
	};

private:
	IDXGISwapChain* _swapChain;
	ID3D11Device* _device;
	ID3D11DeviceContext* _deviceContext;
	ID3D11RenderTargetView* _renderTargetView;
	ID3D11Texture2D* _depthStencilBuffer;
	ID3D11DepthStencilView* _depthStencilView;

	SharedPtr<Material> _lastUsedMaterial;
	SharedPtr<GameObject> _currentlyRenderedObject;
	bool _vsync;

	bool _isResizing;

public:
	Graphics();

private:
	bool GetRefreshRate(uint32 windowHeight, uint32& numerator, uint32& denominator);

	bool InitializeWindowDependentResources(const Window& window);
	void ReleaseWindowDependentResources();

public:
	bool Initialize(bool vsync);
	void Shutdown();

	void BeginScene(D3D11_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	void EndScene();

	void BeginResize();
	void OnResize();
	void EndResize();

	bool CreateBuffer(const D3D11_BUFFER_DESC& bufferDesc, const D3D11_SUBRESOURCE_DATA& bufferData, ID3D11Buffer** bufferPtr);
	bool CreateVertexShader(ID3D10Blob* shaderBuffer, ID3D11VertexShader** vertexShader);
	bool CreatePixelShader(ID3D10Blob* shaderBuffer, ID3D11PixelShader** pixelShader);
	bool CreateInputLayout(D3D11_INPUT_ELEMENT_DESC const* inputLayoutDesc, uint8 inputLayoutDescSize, void* shaderBufferPointer, uint64 shaderBufferSize, ID3D11InputLayout** inputLayout);

	void* Map(ID3D11Resource* resource, D3D11_MAP mapFlag = D3D11_MAP_WRITE_DISCARD);
	void Unmap(ID3D11Resource* resource);
	void SetVSConstantBuffers(uint32 bufferSlot, uint32 bufferCount, ID3D11Buffer** buffers);

	void SetObject(SharedPtr<GameObject> gameObject);
	void SetMaterial(SharedPtr<Material> material);
	void DrawIndexed(ID3D11Buffer* vertexBuffer, ID3D11Buffer* indexBuffer, uint32 indicesCount, uint32 stride, uint32 offset);

	bool CreateRenderState(UniquePtr<RenderState>& renderState);
	bool CreateRenderState(UniquePtr<RenderState>& renderState, const RenderStateParams& renderStateParams);
	void SetRenderState(const RenderState& renderState);
	void SetRenderState(const UniquePtr<RenderState>& renderState);
};

#define RELEASE_COM(comObject)	\
if(comObject)					\
{								\
	comObject->Release();		\
	comObject = 0;				\
}

#define HR(result)	\
if(FAILED(result))	\
{					\
	return false;	\
}
