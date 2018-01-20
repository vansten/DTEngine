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
class MaterialParametersCollection;

enum class ConstantBufferType : uint8
{
	PerFrame,
	PerObject,
	PerDrawCall
};

struct ShaderVariable
{
public:
	String Name;
	uint32 Offset;
	uint32 Size;
};

struct ShaderConstantBuffer
{
private:
	ID3D11Buffer* _constantBuffer;

public:
	String Name;
	uint8 Index;
	ConstantBufferType BufferType;

	DynamicArray<UniquePtr<ShaderVariable>> Variables;

	bool Initialize(Graphics& graphics);
	void Shutdown();

	void Update(Graphics& graphics, const MaterialParametersCollection& materialParametersCollection);
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

	ID3D10Blob* _vertexShaderBuffer;
	ID3D10Blob* _pixelShaderBuffer;

	Dictionary<ConstantBufferType, DynamicArray<UniquePtr<ShaderConstantBuffer>>> _constantBuffers;

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

	void UpdatePerFrameBuffers(Graphics& graphics, const MaterialParametersCollection& materialParametersCollection);
	void UpdatePerObjectBuffers(Graphics& graphics, const MaterialParametersCollection& materialParametersCollection);
	void UpdatePerDrawCallBuffers(Graphics& graphics, const MaterialParametersCollection& materialParametersCollection);

	inline ID3D11InputLayout* GetInputLayout() const { return _inputLayout; }
	inline ID3D11VertexShader* GetVertexShader() const { return _vertexShader; }
	inline ID3D11PixelShader* GetPixelShader() const { return _pixelShader; }
};