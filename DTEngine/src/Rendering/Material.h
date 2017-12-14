#pragma once

#include "Shader.h"

enum class RenderQueue
{
	Opaque,
	Transparent
};

class Material : public Asset
{
protected:
	static const uint16 OpaqueUpperLimit = 1000;

	ID3D11Buffer* _perFrameBuffer;

	std::shared_ptr<Shader> _shader;
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

		return RenderQueue::Transparent;
	}

	inline const XMFLOAT4& GetColor() const { return _color; }
	inline void SetColor(const XMFLOAT4& newColor) { _color = newColor; }

	inline const std::shared_ptr<Shader> GetShader() const { return _shader; }
	inline void SetShader(std::shared_ptr<Shader> shader) {_shader = shader;}
};