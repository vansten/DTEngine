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

class Material final : public Asset
{
private:
	static const unsigned short OPAQUE_UPPER_LIMIT = 1000;
	static const unsigned short TRANSPARENT_UPPER_LIMIT = 2000;

private:
	UniquePtr<RenderState> _renderState;
	SharedPtr<Shader> _shader;
	Vector4 _color;

	unsigned short _queue;
	RenderStateParams _renderStateParams;

	MaterialParametersCollection _parametersCollection;

public:
	Material();
	Material(const Material& other);
	virtual ~Material();

	virtual bool Load(const String& path) override;
	virtual bool Save(const String& path) override;

	virtual bool Initialize() override;
	virtual void Shutdown() override;

	void UpdatePerFrameBuffers(Graphics& graphics);
	void UpdatePerDrawCallBuffers(Graphics& graphics);

	SharedPtr<Material> CreateInstance() const;

	inline RenderQueue GetRenderQueue() const
	{
		if (_queue <= OPAQUE_UPPER_LIMIT)
		{
			return RenderQueue::Opaque;
		}
		if (_queue <= TRANSPARENT_UPPER_LIMIT)
		{
			return RenderQueue::Transparent;
		}

		return RenderQueue::Overlay;
	}

	inline const Vector4& GetColor() const
	{
		return _color;
	}
	inline void SetColor(const Vector4& newColor)
	{
		_color = newColor;
		SetColor(DT_TEXT("Color"), _color);
	}

	inline const SharedPtr<Shader> GetShader() const
	{
		return _shader;
	}
	inline void SetShader(SharedPtr<Shader> shader)
	{
		_shader = shader;
	}

	inline const UniquePtr<RenderState>& GetRenderState() const
	{
		return _renderState;
	}

	inline void SetRenderStateParams(const RenderStateParams& params)
	{
		_renderStateParams = params;
	}

	inline void SetFloat(const String& name, float value)
	{
		_parametersCollection.SetFloat(name, value);
	}

	inline void SetInt(const String& name, int value)
	{
		_parametersCollection.SetInt(name, value);
	}

	inline void SetVector(const String& name, const Vector2& vector)
	{
		_parametersCollection.SetVector(name, vector);
	}

	inline void SetVector(const String& name, const Vector3& vector)
	{
		_parametersCollection.SetVector(name, vector);
	}

	inline void SetColor(const String& name, const Vector4& color)
	{
		_parametersCollection.SetColor(name, color);
	}

	inline void SetMatrix(const String& name, const Matrix& matrix)
	{
		_parametersCollection.SetMatrix(name, matrix);
	}

public:
	inline static void SetGlobalFloat(const String& name, float value)
	{
		MaterialParametersCollection::GLOBAL.SetFloat(name, value);
	}

	inline static void SetGlobalInt(const String& name, int value)
	{
		MaterialParametersCollection::GLOBAL.SetInt(name, value);
	}

	inline static void SetGlobalVector(const String& name, const Vector2& vector)
	{
		MaterialParametersCollection::GLOBAL.SetVector(name, vector);
	}

	inline static void SetGlobalVector(const String& name, const Vector3& vector)
	{
		MaterialParametersCollection::GLOBAL.SetVector(name, vector);
	}

	inline static void SetGlobalColor(const String& name, const Vector4& color)
	{
		MaterialParametersCollection::GLOBAL.SetColor(name, color);
	}

	inline static void SetGlobalMatrix(const String& name, const Matrix& matrix)
	{
		MaterialParametersCollection::GLOBAL.SetMatrix(name, matrix);
	}
};