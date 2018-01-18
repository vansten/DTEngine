#pragma once

#include "ResourceManagement/Asset.h"
#include "Utility/Math.h"

struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11InputLayout;
struct ID3D11Buffer;
struct ID3D11DeviceContext;
struct ID3D10Blob;
struct ID3D11ShaderReflectionConstantBuffer;
struct _D3D11_SHADER_INPUT_BIND_DESC;

class Entity;
class Graphics;

struct ShaderVariable
{
public:
	String Name;
	uint32 Offset;
	uint32 Size;
};

struct ShaderConstantBuffer
{
public:
	String Name;
	uint8 Index;

	DynamicArray<UniquePtr<ShaderVariable>> Variables;
};

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

private:
	ID3D11VertexShader* _vertexShader;
	ID3D11PixelShader* _pixelShader;
	ID3D11InputLayout* _inputLayout;
	ID3D11Buffer* _perFrameBuffer;
	ID3D11Buffer* _perObjectBuffer;

	ID3D10Blob* _vertexShaderBuffer;
	ID3D10Blob* _pixelShaderBuffer;

	DynamicArray<UniquePtr<ShaderConstantBuffer>> _constantBuffers;

public:
	Shader();
	virtual ~Shader();

private:
	bool GatherConstantBuffersInfo(ID3D10Blob* compiledShader);
	bool CreateConstantBufferAndVariables(const _D3D11_SHADER_INPUT_BIND_DESC& reflectedResourceDesc, ID3D11ShaderReflectionConstantBuffer* reflectedConstantBuffer);

public:
	virtual bool Load(const String& path) override;

	virtual bool Initialize() override;
	virtual void Shutdown() override;

	void SetPerFrameParameters(Graphics& graphics);
	void SetPerObjectParameters(Graphics& graphics, Entity* gameObject);
	void SetWorldMatrix(Graphics& graphics, const XMMATRIX& worldMatrix);

	inline ID3D11Buffer* const* GetPerFrameBuffer() const { return &_perFrameBuffer; }
	inline ID3D11Buffer* const* GetPerObjectBuffer() const { return &_perObjectBuffer; }
	inline ID3D11InputLayout* GetInputLayout() const { return _inputLayout; }
	inline ID3D11VertexShader* GetVertexShader() const { return _vertexShader; }
	inline ID3D11PixelShader* GetPixelShader() const { return _pixelShader; }
};