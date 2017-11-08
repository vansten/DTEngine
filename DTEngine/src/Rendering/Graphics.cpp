#include "Graphics.h"

#include "Core/Win32/WindowWin32.h"
#include "GameFramework/GameObject.h"
#include "GameFramework/Components/Camera.h"
#include "GameFramework/Components/MeshRenderer.h"
#include "MeshBase.h"
#include "Material.h"

Graphics::Graphics() : _swapChain(nullptr), _device(nullptr), _deviceContext(nullptr), _renderTargetView(nullptr), _depthStencilBuffer(nullptr), _depthStencilState(nullptr), _depthStencilView(nullptr), _rasterizerState(nullptr)
{

}

bool Graphics::GetRefreshRate(uint32 windowHeight, uint32& numerator, uint32& denominator)
{
	// Get factory, adapter, output, modes and display modes list
	IDXGIFactory* factory;
	HRESULT result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	HR(result);

	IDXGIAdapter* adapter;
	result = factory->EnumAdapters(0, &adapter);
	HR(result);

	IDXGIOutput* adapterOutput;
	result = adapter->EnumOutputs(0, &adapterOutput);
	HR(result);

	uint32 numModes;
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	HR(result);

	DXGI_MODE_DESC* displayModeList = new DXGI_MODE_DESC[numModes];
	if(!displayModeList)
	{
		return false;
	}

	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
	HR(result);

	// Check whether display mode height is equal to desired window height and get its refresh rate
	for(uint32 i = 0; i < numModes; ++i)
	{
		if(displayModeList[i].Height == windowHeight)
		{
			numerator = displayModeList[i].RefreshRate.Numerator;
			denominator = displayModeList[i].RefreshRate.Denominator;
		}
	}

	delete[] displayModeList;
	displayModeList = 0;

	adapterOutput->Release();
	adapterOutput = 0;

	adapter->Release();
	adapter = 0;

	factory->Release();
	factory = 0;

	return true;
}

bool Graphics::Initialize(bool vsync)
{
	uint32 numerator;
	uint32 denominator;

	Window& window = GetMainWindow();

	if(!GetRefreshRate((uint32)window.GetHeight(), numerator, denominator))
	{
		return false;
	}
	
	// Create swap chain, device and device context
	DXGI_SWAP_CHAIN_DESC swapChainDesc = {0};
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Width = (uint32)window.GetWidth();
	swapChainDesc.BufferDesc.Height = (uint32)window.GetHeight();
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	_vsync = vsync;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = _vsync ? numerator : 0;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = _vsync ? denominator : 1;

	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	WindowWin32 windowWin32 = (WindowWin32&)window;
	swapChainDesc.OutputWindow = windowWin32.GetHWND();

	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	swapChainDesc.Windowed = true;

	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

#if DT_DEBUG
	HRESULT result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_DEBUG, &featureLevel, 1, D3D11_SDK_VERSION, &swapChainDesc, &_swapChain, &_device, NULL, &_deviceContext);
#else
	HRESULT result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1, D3D11_SDK_VERSION, &swapChainDesc, &_swapChain, &_device, NULL, &_deviceContext);
#endif
	HR(result);

	ID3D11Texture2D* backBuffer = 0;
	result = _swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
	HR(result);

	// Create render target view
	result = _device->CreateRenderTargetView(backBuffer, NULL, &_renderTargetView);
	HR(result);

	backBuffer->Release();
	backBuffer = 0;

	// Create depth buffer
	D3D11_TEXTURE2D_DESC depthBufferDesc = {0};
	depthBufferDesc.Width = (uint32)window.GetWidth();
	depthBufferDesc.Height = (uint32)window.GetHeight();
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	result = _device->CreateTexture2D(&depthBufferDesc, NULL, &_depthStencilBuffer);
	HR(result);

	// Create depth stencil state
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {0};

	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	result = _device->CreateDepthStencilState(&depthStencilDesc, &_depthStencilState);
	HR(result);

	_deviceContext->OMSetDepthStencilState(_depthStencilState, 1);

	// Create depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	// Cause cannot write = {0} in above line
	ZeroMemory(&depthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));

	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	result = _device->CreateDepthStencilView(_depthStencilBuffer, &depthStencilViewDesc, &_depthStencilView);
	HR(result);

	_deviceContext->OMSetRenderTargets(1, &_renderTargetView, _depthStencilView);

	// Create rasterizer
	D3D11_RASTERIZER_DESC rasterizerDesc;
	// Cause cannot write = {0} in above line
	ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));

	rasterizerDesc.CullMode = D3D11_CULL_BACK;
	rasterizerDesc.DepthClipEnable = true;
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;

	result = _device->CreateRasterizerState(&rasterizerDesc, &_rasterizerState);
	HR(result);

	_deviceContext->RSSetState(_rasterizerState);

	// Create viewport
	D3D11_VIEWPORT viewport = {0};
	viewport.Width = (float32)window.GetWidth();
	viewport.Height = (float32)window.GetHeight();
	viewport.MaxDepth = 1.0f;

	_deviceContext->RSSetViewports(1, &viewport);

	return true;
}

void Graphics::Shutdown()
{
	if(_swapChain)
	{
		// Set to windowed before shutdown
		_swapChain->SetFullscreenState(false, NULL);
	}

	RELEASE_COM(_rasterizerState);
	RELEASE_COM(_depthStencilView);
	RELEASE_COM(_depthStencilState);
	RELEASE_COM(_depthStencilBuffer);
	RELEASE_COM(_renderTargetView);
	RELEASE_COM(_deviceContext);
	RELEASE_COM(_device);
	RELEASE_COM(_swapChain);
}

void Graphics::BeginScene(D3D11_PRIMITIVE_TOPOLOGY topology)
{
	static float color[4] = {0.0f, 0.0f, 0.0f, 1.0f};
	_deviceContext->ClearRenderTargetView(_renderTargetView, color);

	_deviceContext->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	_deviceContext->IASetPrimitiveTopology(topology);

	_lastUsedMaterial = nullptr;
}

void Graphics::EndScene()
{
	if(_vsync)
	{
		_swapChain->Present(1, 0);
	}
	else
	{
		_swapChain->Present(0, 0);
	}
}

bool Graphics::CreateBuffer(const D3D11_BUFFER_DESC& bufferDesc, const D3D11_SUBRESOURCE_DATA& bufferData, ID3D11Buffer** bufferPtr)
{
	if (!bufferPtr || !_device)
	{
		return false;
	}

	HRESULT result = _device->CreateBuffer(&bufferDesc, &bufferData, bufferPtr);
	HR(result);
	return true;
}

bool Graphics::CreateVertexShader(ID3D10Blob* shaderBuffer, ID3D11VertexShader** vertexShader)
{
	if (!shaderBuffer || !vertexShader || !_device)
	{
		return false;
	}

	HRESULT result = _device->CreateVertexShader(shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), nullptr, vertexShader);
	HR(result);
	return true;
}

bool Graphics::CreatePixelShader(ID3D10Blob* shaderBuffer, ID3D11PixelShader** pixelShader)
{
	if (!shaderBuffer || !pixelShader || !_device)
	{
		return false;
	}

	HRESULT result = _device->CreatePixelShader(shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), nullptr, pixelShader);
	HR(result);
	return true;
}

bool Graphics::CreateInputLayout(D3D11_INPUT_ELEMENT_DESC const* inputLayoutDesc, uint8 inputLayoutDescSize, void* shaderBufferPointer, uint64 shaderBufferSize, ID3D11InputLayout** inputLayout)
{
	if (!inputLayoutDesc || !inputLayout || !_device || !shaderBufferPointer || inputLayoutDescSize == 0 || shaderBufferSize == 0)
	{
		return false;
	}

	HRESULT result = _device->CreateInputLayout(inputLayoutDesc, inputLayoutDescSize, shaderBufferPointer, shaderBufferSize, inputLayout);
	HR(result);
	return true;
}

void* Graphics::Map(ID3D11Resource* resource, D3D11_MAP mapFlag)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT result = _deviceContext->Map(resource, 0, mapFlag, 0, &mappedResource);
	if(FAILED(result))
	{
		Unmap(resource);
		return nullptr;
	}

	return mappedResource.pData;
}

void Graphics::Unmap(ID3D11Resource* resource)
{
	_deviceContext->Unmap(resource, 0);
}

void Graphics::SetVSConstantBuffers(uint32 bufferSlot, uint32 bufferCount, ID3D11Buffer** buffers)
{
	_deviceContext->VSSetConstantBuffers(bufferSlot, bufferCount, buffers);
}

void Graphics::SetObject(SharedPtr<GameObject> gameObject)
{
	_currentlyRenderedObject = gameObject;
}

void Graphics::SetMaterial(SharedPtr<Material> material)
{
	if (_lastUsedMaterial != material)
	{
		_lastUsedMaterial = material;
		if (_lastUsedMaterial)
		{
			_lastUsedMaterial->SetPerFrameParameters(*this);
			std::shared_ptr<Shader> shader = material->GetShader();
			_deviceContext->IASetInputLayout(shader->GetInputLayout());
			_deviceContext->VSSetShader(shader->GetVertexShader(), nullptr, 0);
			_deviceContext->PSSetShader(shader->GetPixelShader(), nullptr, 0);
		}
	}
	
	if (_lastUsedMaterial && _currentlyRenderedObject)
	{
		_lastUsedMaterial->SetPerObjectParameters(*this, _currentlyRenderedObject);
	}
}

void Graphics::DrawIndexed(ID3D11Buffer* vertexBuffer, ID3D11Buffer* indexBuffer, uint32 indicesCount, uint32 stride, uint32 offset)
{
	_deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	_deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	_deviceContext->DrawIndexed(indicesCount, 0, 0);
}
