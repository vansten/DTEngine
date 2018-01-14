#pragma once

#include "Shader.h"

struct RenderState;

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

	ID3D11Buffer* _perFrameBuffer;

	UniquePtr<RenderState> _renderState;
	SharedPtr<Shader> _shader;
	XMFLOAT4 _color;

	uint16 _queue;

public:
	Material();
	Material(SharedPtr<Material> other);
	virtual ~Material();

	virtual bool Initialize(const String& path) override;
	virtual void Shutdown() override;

	void SetPerFrameParameters(Graphics& graphics);
	void SetPerObjectParameters(Graphics& graphics, SharedPtr<GameObject> gameObject);
	void SetWorldMatrix(Graphics& graphics, const XMMATRIX& worldMatrix);

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
	inline void SetColor(const XMFLOAT4& newColor) { _color = newColor; }

	inline const SharedPtr<Shader> GetShader() const { return _shader; }
	inline void SetShader(SharedPtr<Shader> shader) {_shader = shader;}

	inline const UniquePtr<RenderState>& GetRenderState() const { return _renderState; }
};