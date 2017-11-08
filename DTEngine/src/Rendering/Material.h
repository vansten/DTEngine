#pragma once

#include "Shader.h"

class Material : public Asset
{
protected:
	ID3D11Buffer* _perFrameBuffer;

	std::shared_ptr<Shader> _shader;
	XMFLOAT4 _color;

public:
	Material();
	virtual ~Material();

	bool Initialize(const String& path);
	virtual void Shutdown() override;

	void SetPerFrameParameters(Graphics& graphics);
	void SetPerObjectParameters(Graphics& graphics, SharedPtr<GameObject> gameObject);

	inline const XMFLOAT4& GetColor() const { return _color; }
	inline void SetColor(const XMFLOAT4& newColor) { _color = newColor; }

	const std::shared_ptr<Shader> GetShader() const { return _shader; }
	void SetShader(std::shared_ptr<Shader> shader) {_shader = shader;}
};