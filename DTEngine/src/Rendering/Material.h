#pragma once

#include "Shader.h"

class Material : public Asset
{
protected:
	ID3D11Buffer* _perFrameBuffer;

	Shader* _shader;
	XMFLOAT4 _color;

public:
	Material();
	virtual ~Material();

	bool Initialize(const string& path);
	virtual void Shutdown() override;

	void SetPerFrameParameters(ID3D11DeviceContext* deviceContext);
	void SetPerObjectParameters(ID3D11DeviceContext* deviceContext, GameObject* gameObject);

	inline const XMFLOAT4& GetColor() const { return _color; }
	inline void SetColor(const XMFLOAT4& newColor) { _color = newColor; }

	const Shader* GetShader() const { return _shader; }
};