#pragma once

#include <d3d11.h>

#include "Utility/EnumInfo.h"

enum class CullMode
{
	None = D3D11_CULL_NONE,
	Back = D3D11_CULL_BACK,
	Front = D3D11_CULL_FRONT
};

enum class FillMode
{
	Wireframe = D3D11_FILL_WIREFRAME,
	Solid = D3D11_FILL_SOLID
};

enum class ZWrite
{
	Off = D3D11_DEPTH_WRITE_MASK_ZERO,
	On = D3D11_DEPTH_WRITE_MASK_ALL
};

enum class BlendMode
{
	Zero = D3D11_BLEND_ZERO,
	One = D3D11_BLEND_ONE,
	SrcColor = D3D11_BLEND_SRC_COLOR,
	InvSrcColor = D3D11_BLEND_INV_SRC_COLOR,
	SrcAlpha = D3D11_BLEND_SRC_ALPHA,
	InvSrcAlpha = D3D11_BLEND_INV_SRC_ALPHA,
	DestAlpha = D3D11_BLEND_DEST_ALPHA,
	InvDestAlpha = D3D11_BLEND_INV_DEST_ALPHA,
	DestColor = D3D11_BLEND_DEST_COLOR,
	InvDestColor = D3D11_BLEND_INV_DEST_COLOR,
	SrcAlphaSat = D3D11_BLEND_SRC_ALPHA_SAT,
	BlendFactor = D3D11_BLEND_BLEND_FACTOR,
	InvBlendFactor = D3D11_BLEND_INV_BLEND_FACTOR,
	Src1Color = D3D11_BLEND_SRC1_COLOR,
	InvSrc1Color = D3D11_BLEND_INV_SRC1_COLOR,
	Src1Alpha = D3D11_BLEND_SRC1_ALPHA,
	InvSrc1Alpha = D3D11_BLEND_INV_SRC1_ALPHA
};

enum class CompareFunction
{
	Never = D3D11_COMPARISON_NEVER,
	Equal = D3D11_COMPARISON_EQUAL,
	NotEqual = D3D11_COMPARISON_NOT_EQUAL,
	Less = D3D11_COMPARISON_LESS,
	LessOrEqual = D3D11_COMPARISON_LESS_EQUAL,
	Greater = D3D11_COMPARISON_GREATER,
	GreaterOrEqual = D3D11_COMPARISON_GREATER_EQUAL,
	Always = D3D11_COMPARISON_ALWAYS
};

DECLARE_ENUM_NAMES(CullMode)
{
	{
		DT_TEXT("None"), CullMode::None
	},
		{DT_TEXT("Back"), CullMode::Back},
		{DT_TEXT("Front"), CullMode::Front}
};

DECLARE_ENUM_NAMES(FillMode)
{
	{
		DT_TEXT("Wireframe"), FillMode::Wireframe
	},
		{DT_TEXT("Solid"), FillMode::Solid}
};

DECLARE_ENUM_NAMES(ZWrite)
{
	{
		DT_TEXT("Off"), ZWrite::Off
	},
		{DT_TEXT("On"), ZWrite::On}
};

DECLARE_ENUM_NAMES(BlendMode)
{
	{
		DT_TEXT("Zero"), BlendMode::Zero
	},
		{DT_TEXT("One"), BlendMode::One},
		{DT_TEXT("SrcColor"), BlendMode::SrcColor},
		{DT_TEXT("InvSrcColor"), BlendMode::InvSrcColor},
		{DT_TEXT("SrcAlpha"), BlendMode::SrcAlpha},
		{DT_TEXT("InvSrcAlpha"), BlendMode::InvSrcAlpha},
		{DT_TEXT("DestAlpha"), BlendMode::DestAlpha},
		{DT_TEXT("InvDestAlpha"), BlendMode::InvDestAlpha},
		{DT_TEXT("DestColor"), BlendMode::InvDestColor},
		{DT_TEXT("SrcAlphaSat"), BlendMode::SrcAlphaSat},
		{DT_TEXT("BlendFactor"), BlendMode::BlendFactor},
		{DT_TEXT("InvBlendFactor"), BlendMode::InvBlendFactor},
		{DT_TEXT("Src1Color"), BlendMode::Src1Color},
		{DT_TEXT("InvSrc1Color"), BlendMode::InvSrc1Color},
		{DT_TEXT("Src1Alpha"), BlendMode::Src1Alpha},
		{DT_TEXT("InvSrc1Alpha"), BlendMode::InvSrc1Alpha}
};

DECLARE_ENUM_NAMES(CompareFunction)
{
	{
		DT_TEXT("Never"), CompareFunction::Never
	},
		{DT_TEXT("Equal"), CompareFunction::Equal},
		{DT_TEXT("NotEqual"), CompareFunction::NotEqual},
		{DT_TEXT("Less"), CompareFunction::Less},
		{DT_TEXT("LessOrEqual"), CompareFunction::LessOrEqual},
		{DT_TEXT("Greater"), CompareFunction::Greater},
		{DT_TEXT("GreaterOrEqual"), CompareFunction::GreaterOrEqual},
		{DT_TEXT("Always"), CompareFunction::Always}
};

struct RenderStateParams final
{
	friend struct RenderState;

private:
	CullMode _cullMode;
	FillMode _fillMode;
	ZWrite _zWrite;
	BlendMode _srcBlendMode;
	BlendMode _destBlendMode;
	CompareFunction _zTestFunction;

public:
	inline RenderStateParams() : _cullMode(CullMode::Back), _fillMode(FillMode::Solid), _zWrite(ZWrite::On), _srcBlendMode(BlendMode::SrcAlpha), _destBlendMode(BlendMode::InvSrcAlpha), _zTestFunction(CompareFunction::Less)
	{}
	inline RenderStateParams(CullMode cullMode, FillMode fillMode, ZWrite zWrite, BlendMode srcBlendMode, BlendMode destBlendMode, CompareFunction zTestFunction) : _cullMode(cullMode), _fillMode(fillMode), _zWrite(zWrite), _srcBlendMode(srcBlendMode), _destBlendMode(destBlendMode), _zTestFunction(zTestFunction)
	{}
	inline RenderStateParams(const RenderStateParams& other) : _cullMode(other._cullMode), _fillMode(other._fillMode), _zWrite(other._zWrite), _srcBlendMode(other._srcBlendMode), _destBlendMode(other._destBlendMode), _zTestFunction(other._zTestFunction)
	{}

	inline CullMode GetCullMode() const
	{
		return _cullMode;
	}

	inline FillMode GetFillMode() const
	{
		return _fillMode;
	}

	inline ZWrite GetZWrite() const
	{
		return _zWrite;
	}

	inline BlendMode GetSrcBlendMode() const
	{
		return _srcBlendMode;
	}

	inline BlendMode GetDestBlendMode() const
	{
		return _destBlendMode;
	}

	inline CompareFunction GetZTestFunction() const
	{
		return _zTestFunction;
	}

	inline RenderStateParams& operator=(const RenderStateParams& other)
	{
		_cullMode = other._cullMode;
		_fillMode = other._fillMode;
		_zWrite = other._zWrite;
		_srcBlendMode = other._srcBlendMode;
		_destBlendMode = other._destBlendMode;
		_zTestFunction = other._zTestFunction;

		return *this;
	}

	inline bool operator==(const RenderStateParams& other) const
	{
		return _cullMode == other._cullMode && _fillMode == other._fillMode &&
			_zWrite == other._zWrite && _srcBlendMode == other._srcBlendMode &&
			_destBlendMode == other._destBlendMode && _zTestFunction == other._zTestFunction;
	}

	inline bool operator!=(const RenderStateParams& other) const
	{
		return _cullMode != other._cullMode || _fillMode != other._fillMode ||
			_zWrite != other._zWrite || _srcBlendMode != other._srcBlendMode ||
			_destBlendMode != other._destBlendMode || _zTestFunction != other._zTestFunction;
	}
};

struct RenderState final
{
	friend class Graphics;

private:
	RenderStateParams _params;

	ID3D11DepthStencilState* _depthStencilState;
	ID3D11RasterizerState* _rasterizerState;

private:
	inline RenderState(const RenderStateParams& renderStateParams) : _params(renderStateParams), _depthStencilState(nullptr), _rasterizerState(nullptr)
	{}
	inline RenderState() : _depthStencilState(nullptr), _rasterizerState(nullptr)
	{}
	inline RenderState(const RenderState& other) = delete;
	inline RenderState(RenderState&& other) = delete;

	bool Initialize(ID3D11Device* device);

public:
	void Shutdown();

	inline bool operator==(const RenderState& other) const
	{
		return _params == other._params;
	}

	inline bool operator!=(const RenderState& other) const
	{
		return _params != other._params;
	}

	inline RenderState& operator=(const RenderState& other) = delete;
};
