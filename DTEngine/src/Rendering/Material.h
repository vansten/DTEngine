#pragma once

#include "Shader.h"
#include "RenderState.h"
#include "MaterialParametersCollection.h"

enum class RenderQueue
{
	// Normal geometry
	Opaque,
	// Transparent geometry
	Transparent,
	// UI
	Overlay
};

class Material : public Asset
{
protected:
	static const uint16 OpaqueUpperLimit = 1000;
	static const uint16 TransparentUpperLimit = 2000;

	UniquePtr<RenderState> _renderState;
	SharedPtr<Shader> _shader;
	XMFLOAT4 _color;

	uint16 _queue;
	RenderStateParams _renderStateParams;

	MaterialParametersCollection _parametersCollection;

public:
	Material();
	virtual ~Material();

	virtual bool Load(const String& path) override;
	virtual bool Save(const String& path) override;

	virtual bool Initialize() override;
	virtual void Shutdown() override;

	void UpdatePerFrameBuffers(Graphics& graphics);
	void UpdatePerObjectBuffers(Graphics& graphics);
	void UpdatePerDrawCallBuffers(Graphics& graphics);

	inline RenderQueue GetRenderQueue() const
	{
		if(_queue <= OpaqueUpperLimit)
		{
			return RenderQueue::Opaque;
		}
		if(_queue <= TransparentUpperLimit)
		{
			return RenderQueue::Transparent;
		}

		return RenderQueue::Overlay;
	}

	inline const XMFLOAT4& GetColor() const { return _color; }
	inline void SetColor(const XMFLOAT4& newColor) 
	{
		_color = newColor;
		SetColor(DT_TEXT("Color"), _color);
	}

	inline const SharedPtr<Shader> GetShader() const { return _shader; }
	inline void SetShader(SharedPtr<Shader> shader) {_shader = shader;}

	inline const UniquePtr<RenderState>& GetRenderState() const { return _renderState; }

	inline void SetRenderStateParams(const RenderStateParams& params) { _renderStateParams = params; }

	inline void SetFloat(const String& name, float value)
	{
		_parametersCollection.SetFloat(name, value);
	}

	inline void SetInt(const String& name, int value)
	{
		_parametersCollection.SetInt(name, value);
	}

	inline void SetVector(const String& name, const XMFLOAT2& vector)
	{
		_parametersCollection.SetVector(name, vector);
	}

	inline void SetVector(const String& name, const XMFLOAT3& vector)
	{
		_parametersCollection.SetVector(name, vector);
	}

	inline void SetVector(const String& name, const XMFLOAT4& vector)
	{
		_parametersCollection.SetVector(name, vector);
	}
	
	inline void SetColor(const String& name, const XMFLOAT4& color)
	{
		_parametersCollection.SetColor(name, color);
	}

	inline void SetMatrix(const String& name, const XMMATRIX& matrix)
	{
		_parametersCollection.SetMatrix(name, matrix);
	}
};