#include "Graphics.h"

#include "Core/Win32/WindowWin32.h"

#include "Debug/Debug.h"

#include "GameFramework/GameObject.h"
#include "GameFramework/Components/Camera.h"
#include "GameFramework/Components/MeshRenderer.h"

#include "MeshBase.h"
#include "Material.h"

RenderState Graphics::CommonRenderStates::WireframeRenderState(RenderStateParams(CullMode::None, FillMode::Wireframe, ZWrite::Off, BlendMode::One, BlendMode::Zero, CompareFunction::Always));
RenderState Graphics::CommonRenderStates::DefaultRenderState(RenderStateParams(CullMode::Back, FillMode::Solid, ZWrite::On, BlendMode::SrcAlpha, BlendMode::InvSrcAlpha, CompareFunction::Less));

bool RenderState::Initialize(ID3D11Device* device)
{
	if(!device)
	{
		return false;
	}

	HRESULT result;
	
	// Create depth stencil state
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {0};

	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = (D3D11_DEPTH_WRITE_MASK)_params._zWrite;
	depthStencilDesc.DepthFunc = (D3D11_COMPARISON_FUNC)_params._zTestFunction;

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

	result = device->CreateDepthStencilState(&depthStencilDesc, &_depthStencilState);
	HR(result);

	// Create rasterizer
	D3D11_RASTERIZER_DESC rasterizerDesc;
	// Cause cannot write = {0} in above line
	ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));

	rasterizerDesc.CullMode = (D3D11_CULL_MODE)_params._cullMode;
	rasterizerDesc.DepthClipEnable = true;
	rasterizerDesc.FillMode = (D3D11_FILL_MODE)_params._fillMode;

	result = device->CreateRasterizerState(&rasterizerDesc, &_rasterizerState);
	HR(result);

	return true;
}

void RenderState::Shutdown()
{
	RELEASE_COM(_rasterizerState);
	RELEASE_COM(_depthStencilState);
}

bool Graphics::CommonRenderStates::Initialize(ID3D11Device* device)
{
	return WireframeRenderState.Initialize(device) && DefaultRenderState.Initialize(device);
}

void Graphics::CommonRenderStates::Shutdown()
{
	WireframeRenderState.Shutdown();
	DefaultRenderState.Shutdown();
}

Graphics::Graphics() : _swapChain(nullptr), _device(nullptr), _deviceContext(nullptr), _renderTargetView(nullptr), _depthStencilBuffer(nullptr), _depthStencilView(nullptr)
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

bool Graphics::InitializeWindowDependentResources(const Window& window)
{
	uint32 numerator;
	uint32 denominator;

	const uint32 windowWidth = (uint32)window.GetWidth();
	const uint32 windowHeight = (uint32)window.GetHeight();

	if(!GetRefreshRate(windowHeight, numerator, denominator))
	{
		GetDebug().Print(LogVerbosity::Error, CHANNEL_GRAPHICS, DT_TEXT("Cannot retrieve refresh rate"));
		return false;
	}

	IDXGIDevice* DXGIDevice = nullptr;
	_device->QueryInterface(__uuidof(IDXGIDevice), (void **)&DXGIDevice);

	IDXGIAdapter* DXGIAdapter = nullptr;
	DXGIDevice->GetAdapter(&DXGIAdapter);

	IDXGIFactory* DXGIFactory = nullptr;
	DXGIAdapter->GetParent(__uuidof(DXGIFactory), (void **)&DXGIFactory);

	// Create swap chain, device and device context
	DXGI_SWAP_CHAIN_DESC swapChainDesc = {0};
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Width = windowWidth;
	swapChainDesc.BufferDesc.Height = windowHeight;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	swapChainDesc.BufferDesc.RefreshRate.Numerator = _vsync ? numerator : 0;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = _vsync ? denominator : 1;

	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	const WindowWin32& windowWin32 = (const WindowWin32&)window;
	swapChainDesc.OutputWindow = windowWin32.GetHWND();

	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	swapChainDesc.Windowed = true;

	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	HRESULT result = DXGIFactory->CreateSwapChain(_device, &swapChainDesc, &_swapChain);

	RELEASE_COM(DXGIFactory);
	RELEASE_COM(DXGIAdapter);
	RELEASE_COM(DXGIDevice);

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
	depthBufferDesc.Width = windowWidth;
	depthBufferDesc.Height = windowHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	result = _device->CreateTexture2D(&depthBufferDesc, NULL, &_depthStencilBuffer);
	HR(result);

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

	if(!CommonRenderStates::Initialize(_device))
	{
		return false;
	}

	SetRenderState(CommonRenderStates::DefaultRenderState);

	// Create viewport
	D3D11_VIEWPORT viewport = {0};
	viewport.Width = (float32)windowWidth;
	viewport.Height = (float32)windowHeight;
	viewport.MaxDepth = 1.0f;

	_deviceContext->RSSetViewports(1, &viewport);

	return true;
}

void Graphics::ReleaseWindowDependentResources()
{
	if(_swapChain)
	{
		// Set to windowed before shutdown
		_swapChain->SetFullscreenState(false, NULL);
	}
	CommonRenderStates::Shutdown();
	RELEASE_COM(_depthStencilView);
	RELEASE_COM(_depthStencilBuffer);
	RELEASE_COM(_renderTargetView);
	RELEASE_COM(_swapChain);
}

bool Graphics::Initialize(bool vsync)
{
	_vsync = vsync;

	const D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

	uint32 deviceFlag = 0;

#if DT_DEBUG
	deviceFlag |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	HRESULT result = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, deviceFlag, &featureLevel, 1, D3D11_SDK_VERSION, &_device, nullptr, &_deviceContext);

	HR(result);

	if(!InitializeWindowDependentResources(GetMainWindow()))
	{
		GetDebug().Print(LogVerbosity::Error, CHANNEL_GRAPHICS, DT_TEXT("Cannot initialize window size dependent resources"));
		return false;
	}

	return true;
}

void Graphics::Shutdown()
{
	ReleaseWindowDependentResources();
	RELEASE_COM(_deviceContext);
	RELEASE_COM(_device);
}

void Graphics::BeginScene(D3D11_PRIMITIVE_TOPOLOGY topology)
{
	static const float color[4] = {0.0f, 0.0f, 0.0f, 1.0f};
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

void Graphics::BeginResize()
{
	_isResizing = true;
}

void Graphics::OnResize()
{
	if(_isResizing)
	{
		return;
	}

	uint16 width = GetMainWindow().GetWidth();
	uint16 height = GetMainWindow().GetHeight();
	GetDebug().Printf(LogVerbosity::Log, CHANNEL_GRAPHICS, DT_TEXT("Resizing... New size is %i X %i"), width, height);

	ReleaseWindowDependentResources();
	InitializeWindowDependentResources(GetMainWindow());

	Camera::OnResize();
}

void Graphics::EndResize()
{
	_isResizing = false;
	OnResize();
}

bool Graphics::CreateBuffer(const D3D11_BUFFER_DESC& bufferDesc, const D3D11_SUBRESOURCE_DATA& bufferData, ID3D11Buffer** bufferPtr)
{
	if (!bufferPtr || !_device)
	{
		GetDebug().Print(LogVerbosity::Error, CHANNEL_GRAPHICS, DT_TEXT("Failed to create buffer. Either bufferPtr or device is nullptr"));
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
		GetDebug().Print(LogVerbosity::Error, CHANNEL_GRAPHICS, DT_TEXT("Failed to create vertex shader. Either shaderBuffer, vertexShader or device is nullptr"));
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
		GetDebug().Print(LogVerbosity::Error, CHANNEL_GRAPHICS, DT_TEXT("Failed to create vertex shader. Either shaderBuffer, pixelShader or device is nullptr"));
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
		GetDebug().Print(LogVerbosity::Error, CHANNEL_GRAPHICS, DT_TEXT("Failed to map resources"));
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
			SharedPtr<Shader> shader = material->GetShader();
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

bool Graphics::CreateRenderState(UniquePtr<RenderState>& renderState)
{
	if(renderState != nullptr)
	{
		GetDebug().Print(LogVerbosity::Warning, CHANNEL_GRAPHICS, DT_TEXT("Creating render state in place of existing render state. Possible memory leak!"));
	}

	renderState.reset(new RenderState());

	return renderState->Initialize(_device);
}

bool Graphics::CreateRenderState(UniquePtr<RenderState>& renderState, const RenderStateParams& renderStateParams)
{
	if(renderState != nullptr)
	{
		GetDebug().Print(LogVerbosity::Warning, CHANNEL_GRAPHICS, DT_TEXT("Creating render state in place of existing render state. Possible memory leak!"));
	}

	renderState.reset(new RenderState(renderStateParams));

	return renderState->Initialize(_device);
}

void Graphics::SetRenderState(const RenderState& renderState)
{
	_deviceContext->OMSetDepthStencilState(renderState._depthStencilState, 1);
	_deviceContext->RSSetState(renderState._rasterizerState);
}

void Graphics::SetRenderState(const UniquePtr<RenderState>& renderState)
{
	if(renderState.get() != nullptr)
	{
		const RenderState& renderStateRef = *renderState;
		SetRenderState(renderStateRef);
	}
}
