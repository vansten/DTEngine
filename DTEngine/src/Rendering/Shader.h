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
struct _D3D11_SHADER_TYPE_DESC;

class Entity;
class Graphics;
class MaterialParametersCollection;

struct ShaderVariable
{
	typedef void const* (MaterialParametersCollection::*VariableGetterFunctionPointer)(const String&) const;

public:
	String Name;
	unsigned int Offset;
	unsigned int Size;
	VariableGetterFunctionPointer VariableGetterFunction;

	void SetGetterFunctionFromTypeDescription(const _D3D11_SHADER_TYPE_DESC& typeDescription);
	void const* Get(const MaterialParametersCollection& materialParametersCollection);
};

struct ShaderConstantBuffer
{
private:
	ID3D11Buffer* _constantBuffer;

public:
	String Name;
	unsigned char Index;

	DynamicArray<UniquePtr<ShaderVariable>> Variables;

	bool Initialize(Graphics& graphics);
	void Shutdown();

	void Update(Graphics& graphics, const MaterialParametersCollection& materialParametersCollection);
};

class Shader final : public Asset
{
private:
	ID3D11VertexShader* _vertexShader;
	ID3D11PixelShader* _pixelShader;
	ID3D11InputLayout* _inputLayout;

	ID3D10Blob* _vertexShaderBuffer;
	ID3D10Blob* _pixelShaderBuffer;

	DynamicArray<UniquePtr<ShaderConstantBuffer>> _perFrameBuffers;
	DynamicArray<UniquePtr<ShaderConstantBuffer>> _perDrawCallBuffers;

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
	void UpdatePerDrawCallBuffers(Graphics& graphics, const MaterialParametersCollection& materialParametersCollection);

	inline ID3D11InputLayout* GetInputLayout() const
	{
		return _inputLayout;
	}
	inline ID3D11VertexShader* GetVertexShader() const
	{
		return _vertexShader;
	}
	inline ID3D11PixelShader* GetPixelShader() const
	{
		return _pixelShader;
	}
};