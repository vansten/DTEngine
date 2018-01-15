#include "RenderState.h"

#include "Graphics.h"

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
