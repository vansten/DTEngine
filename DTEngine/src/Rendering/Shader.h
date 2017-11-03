#pragma once

#include "ResourceManagement/Asset.h"
#include "Utility/Math.h"

struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11InputLayout;
struct ID3D11Buffer;
struct ID3D11DeviceContext;

class GameObject;

class Shader : public Asset
{
public:
	struct PerFrameBuffer
	{
		XMMATRIX view;
		XMMATRIX projection;
	};

	struct PerObjectBuffer
	{
		XMMATRIX world;
	};

protected:
	ID3D11VertexShader* _vertexShader;
	ID3D11PixelShader* _pixelShader;
	ID3D11InputLayout* _inputLayout;
	ID3D11Buffer* _perFrameBuffer;
	ID3D11Buffer* _perObjectBuffer;

public:
	Shader();
	virtual ~Shader();

	virtual bool Initialize(const string& path) override;
	virtual void Shutdown() override;

	void SetPerFrameParameters(ID3D11DeviceContext* deviceContext);
	void SetPerObjectParameters(ID3D11DeviceContext* deviceContext, GameObject* gameObject);

	inline ID3D11Buffer* const* GetPerFrameBuffer() const { return &_perFrameBuffer; }
	inline ID3D11Buffer* const* GetPerObjectBuffer() const { return &_perObjectBuffer; }
	inline ID3D11InputLayout* GetInputLayout() const { return _inputLayout; }
	inline ID3D11VertexShader* GetVertexShader() const { return _vertexShader; }
	inline ID3D11PixelShader* GetPixelShader() const { return _pixelShader; }
};