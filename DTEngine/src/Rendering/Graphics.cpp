#include "Graphics.h"

#include "Core/Window.h"

#include "Debug/Debug.h"

#include "GameFramework/Entity.h"
#include "GameFramework/Components/Camera.h"
#include "GameFramework/Components/MeshRenderer.h"

#include "MeshBase.h"
#include "Material.h"

Graphics gGraphics;

RenderState Graphics::CommonRenderStates::WireframeRenderState(RenderStateParams(CullMode::None, FillMode::Wireframe, ZWrite::Off, BlendMode::One, BlendMode::Zero, CompareFunction::Always));
RenderState Graphics::CommonRenderStates::DefaultRenderState(RenderStateParams(CullMode::Back, FillMode::Solid, ZWrite::On, BlendMode::SrcAlpha, BlendMode::InvSrcAlpha, CompareFunction::Less));

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

bool Graphics::GetRefreshRate(unsigned int windowHeight, unsigned int& numerator, unsigned int& denominator)
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

	unsigned int numModes;
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
	for(unsigned int i = 0; i < numModes; ++i)
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
	unsigned int numerator;
	unsigned int denominator;

	const unsigned int windowWidth = (unsigned int)window.GetWidth();
	const unsigned int windowHeight = (unsigned int)window.GetHeight();

	if(!GetRefreshRate(windowHeight, numerator, denominator))
	{
		gDebug.Print(LogVerbosity::Error, CHANNEL_GRAPHICS, DT_TEXT("Cannot retrieve refresh rate"));
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

	swapChainDesc.OutputWindow = gWindow.GetHWND();

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
	viewport.Width = (float)windowWidth;
	viewport.Height = (float)windowHeight;
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

	unsigned int deviceFlag = 0;

#if DT_DEBUG
	deviceFlag |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	HRESULT result = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, deviceFlag, &featureLevel, 1, D3D11_SDK_VERSION, &_device, nullptr, &_deviceContext);

	HR(result);

	if(!InitializeWindowDependentResources(gWindow))
	{
		gDebug.Print(LogVerbosity::Error, CHANNEL_GRAPHICS, DT_TEXT("Cannot initialize window size dependent resources"));
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

	unsigned short width = gWindow.GetWidth();
	unsigned short height = gWindow.GetHeight();
	gDebug.Printf(LogVerbosity::Log, CHANNEL_GRAPHICS, DT_TEXT("Resizing... New size is %i X %i"), width, height);

	ReleaseWindowDependentResources();
	InitializeWindowDependentResources(gWindow);

	Camera::OnResize();
}

void Graphics::EndResize()
{
	_isResizing = false;
	OnResize();
}

bool Graphics::CreateBuffer(const D3D11_BUFFER_DESC& bufferDesc, ID3D11Buffer** bufferPtr)
{
	if(!bufferPtr || !_device)
	{
		gDebug.Print(LogVerbosity::Error, CHANNEL_GRAPHICS, DT_TEXT("Failed to create buffer. Either bufferPtr or device is nullptr"));
		return false;
	}

	HRESULT result = _device->CreateBuffer(&bufferDesc, nullptr, bufferPtr);
	HR(result);
	return true;
}

bool Graphics::CreateBuffer(const D3D11_BUFFER_DESC& bufferDesc, const D3D11_SUBRESOURCE_DATA& bufferData, ID3D11Buffer** bufferPtr)
{
	if (!bufferPtr || !_device)
	{
		gDebug.Print(LogVerbosity::Error, CHANNEL_GRAPHICS, DT_TEXT("Failed to create buffer. Either bufferPtr or device is nullptr"));
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
		gDebug.Print(LogVerbosity::Error, CHANNEL_GRAPHICS, DT_TEXT("Failed to create vertex shader. Either shaderBuffer, vertexShader or device is nullptr"));
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
		gDebug.Print(LogVerbosity::Error, CHANNEL_GRAPHICS, DT_TEXT("Failed to create vertex shader. Either shaderBuffer, pixelShader or device is nullptr"));
		return false;
	}

	HRESULT result = _device->CreatePixelShader(shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), nullptr, pixelShader);
	HR(result);
	return true;
}

bool Graphics::CreateInputLayout(D3D11_INPUT_ELEMENT_DESC const* inputLayoutDesc, unsigned char inputLayoutDescSize, void* shaderBufferPointer, size_t shaderBufferSize, ID3D11InputLayout** inputLayout)
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
		gDebug.Print(LogVerbosity::Error, CHANNEL_GRAPHICS, DT_TEXT("Failed to map resources"));
		Unmap(resource);
		return nullptr;
	}

	return mappedResource.pData;
}

void Graphics::Unmap(ID3D11Resource* resource)
{
	_deviceContext->Unmap(resource, 0);
}

void Graphics::SetVSConstantBuffers(unsigned int bufferSlot, unsigned int bufferCount, ID3D11Buffer** buffers)
{
	_deviceContext->VSSetConstantBuffers(bufferSlot, bufferCount, buffers);
}

void Graphics::SetObject(Entity* entity)
{
	_currentlyRenderedEntity = entity;
}

void Graphics::SetMaterial(Material* material)
{
	if (_lastUsedMaterial != material)
	{
		_lastUsedMaterial = material;
		if (_lastUsedMaterial)
		{
			_lastUsedMaterial->UpdatePerFrameBuffers(*this);
			SharedPtr<Shader> shader = material->GetShader();
			_deviceContext->IASetInputLayout(shader->GetInputLayout());
			_deviceContext->VSSetShader(shader->GetVertexShader(), nullptr, 0);
			_deviceContext->PSSetShader(shader->GetPixelShader(), nullptr, 0);
		}
	}
	
	if (_lastUsedMaterial && _currentlyRenderedEntity)
	{
		static const String MODEL_TO_WORLD_MATRIX_NAME = DT_TEXT("Model2WorldMatrix");
		_lastUsedMaterial->SetMatrix(MODEL_TO_WORLD_MATRIX_NAME, _currentlyRenderedEntity->GetTransform().GetModelMatrix());
		_lastUsedMaterial->UpdatePerObjectBuffers(*this);
	}
}

void Graphics::DrawIndexed(ID3D11Buffer* vertexBuffer, ID3D11Buffer* indexBuffer, unsigned int indicesCount, unsigned int stride, unsigned int offset)
{
	_deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	_deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	_deviceContext->DrawIndexed(indicesCount, 0, 0);
}

bool Graphics::CreateRenderState(UniquePtr<RenderState>& renderState)
{
	if(renderState)
	{
		gDebug.Print(LogVerbosity::Warning, CHANNEL_GRAPHICS, DT_TEXT("Creating render state in place of existing render state. Possible memory leak!"));
	}

	renderState.reset(new RenderState());

	return renderState->Initialize(_device);
}

bool Graphics::CreateRenderState(UniquePtr<RenderState>& renderState, const RenderStateParams& renderStateParams)
{
	if(renderState)
	{
		gDebug.Print(LogVerbosity::Warning, CHANNEL_GRAPHICS, DT_TEXT("Creating render state in place of existing render state. Possible memory leak!"));
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
	if(renderState)
	{
		_deviceContext->OMSetDepthStencilState(renderState->_depthStencilState, 1);
		_deviceContext->RSSetState(renderState->_rasterizerState);
	}
}
