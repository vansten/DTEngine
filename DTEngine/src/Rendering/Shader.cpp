#include "Shader.h"

#include <d3d11.h>
#include <d3dcompiler.h>
#include <d3d11shader.h>

#include "Debug/Debug.h"
#include "Graphics.h"
#include "GameFramework/Entity.h"
#include "GameFramework/Components/Camera.h"
#include "Rendering/MaterialParametersCollection.h"
#include "Utility/String.h"

void ShaderVariable::SetGetterFunctionFromTypeDescription(const _D3D11_SHADER_TYPE_DESC& typeDescription)
{
	VariableGetterFunction = &MaterialParametersCollection::Get;
	if(typeDescription.Class == D3D_SVC_MATRIX_COLUMNS)
	{
		VariableGetterFunction = &MaterialParametersCollection::GetMatrix;
	}
	else if(typeDescription.Class == D3D_SVC_VECTOR)
	{
		unsigned int numberOfElements = typeDescription.Columns == 1 ? typeDescription.Rows : typeDescription.Columns;
		switch(numberOfElements)
		{
		case 1:
			break;
		case 2:
			VariableGetterFunction = &MaterialParametersCollection::GetVector2;
			break;
		case 3:
			VariableGetterFunction = &MaterialParametersCollection::GetVector3;
			break;
		case 4:
			VariableGetterFunction = &MaterialParametersCollection::GetVector4;
			break;
		}
	}
	else if(typeDescription.Class == D3D_SVC_SCALAR)
	{
		switch(typeDescription.Type)
		{
		case D3D_SVT_BOOL:
			break;
		case D3D_SVT_DOUBLE:
			break;
		case D3D_SVT_FLOAT:
			VariableGetterFunction = &MaterialParametersCollection::GetFloat;
			break;
		case D3D_SVT_INT:
			VariableGetterFunction = &MaterialParametersCollection::GetInt;
			break;
		}
	}
	else if(typeDescription.Class == D3D_SVC_OBJECT)
	{
		DT_ASSERT(false, DT_TEXT("Unsupported shader variable type!"));
	}
}

void const* ShaderVariable::Get(const MaterialParametersCollection& materialParametersCollection)
{
	if(VariableGetterFunction == nullptr)
	{
		return nullptr;
	}

	MaterialParametersCollection* rawPtr = const_cast<MaterialParametersCollection*>(&materialParametersCollection);
	return (rawPtr->*VariableGetterFunction)(Name);
}

bool ShaderConstantBuffer::Initialize(Graphics& graphics)
{
	unsigned int bufferSize = 0;
	for(auto& variable : Variables)
	{
		bufferSize += variable->Size;
	}

	const D3D11_SUBRESOURCE_DATA bufferData = {0};

	D3D11_BUFFER_DESC bufferDesc = {0};
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = bufferSize;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	return graphics.CreateBuffer(bufferDesc, &_constantBuffer);
}

void ShaderConstantBuffer::Shutdown()
{
	RELEASE_COM(_constantBuffer);
}

void ShaderConstantBuffer::Update(Graphics& graphics, const MaterialParametersCollection& materialParametersCollection)
{
	void* data = graphics.Map(_constantBuffer);
	if(!data)
	{
		graphics.Unmap(_constantBuffer);
		return;
	}

	for(auto& variable : Variables)
	{
		void const* variableData = variable->Get(materialParametersCollection);
		if(variableData == nullptr)
		{
			variableData = variable->Get(MaterialParametersCollection::GLOBAL);
		}

		if(variableData == nullptr)
		{
			continue;
		}

		void* destPtr = (char*)data + variable->Offset;
		memcpy(destPtr, variableData, variable->Size);
	}

	graphics.Unmap(_constantBuffer);
	graphics.SetVSConstantBuffers(Index, 1, &_constantBuffer);
}

Shader::Shader() : _vertexShader(nullptr), _pixelShader(nullptr), _inputLayout(nullptr)
{
}

Shader::~Shader()
{

}

bool Shader::GatherConstantBuffersInfo(ID3D10Blob* compiledShader)
{
	if(compiledShader == nullptr)
	{
		return false;
	}

	ID3D11ShaderReflection* reflectedShader = nullptr;
	HRESULT result = D3DReflect(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&reflectedShader);
	HR_REACTION(result, gDebug.Print(LogVerbosity::Error, CHANNEL_GRAPHICS, DT_TEXT("Cannot reflect shader!")));

	// Get shader description
	D3D11_SHADER_DESC reflectedShaderDesc;
	reflectedShader->GetDesc(&reflectedShaderDesc);

	// For each of bound resource (textures, samplers, constant buffers)
	for(unsigned int i = 0; i < reflectedShaderDesc.BoundResources; ++i)
	{
		D3D11_SHADER_INPUT_BIND_DESC reflectedBoundResourceDesc;
		result = reflectedShader->GetResourceBindingDesc(i, &reflectedBoundResourceDesc);

		switch(reflectedBoundResourceDesc.Type)
		{
		case D3D_SIT_CBUFFER:
			{
				ID3D11ShaderReflectionConstantBuffer* reflectedConstantBuffer = reflectedShader->GetConstantBufferByName(reflectedBoundResourceDesc.Name);
				CreateConstantBufferAndVariables(reflectedBoundResourceDesc, reflectedConstantBuffer);
			}
			break;
		case D3D_SIT_TEXTURE:
			break;
		case D3D_SIT_SAMPLER:
			break;
		}
	}

	return true;
}

bool Shader::CreateConstantBufferAndVariables(const _D3D11_SHADER_INPUT_BIND_DESC& reflectedResourceDesc, ID3D11ShaderReflectionConstantBuffer* reflectedConstantBuffer)
{
	D3D11_SHADER_BUFFER_DESC reflectedConstantBufferDesc;
	HRESULT result = reflectedConstantBuffer->GetDesc(&reflectedConstantBufferDesc);
	HR_REACTION(result, gDebug.Print(LogVerbosity::Error, CHANNEL_GRAPHICS, DT_TEXT("Cannot obtain constant buffer description!")));

	// Create constant buffer
	UniquePtr<ShaderConstantBuffer> constantBuffer = std::make_unique<ShaderConstantBuffer>();
	const std::string name = reflectedConstantBufferDesc.Name;
	constantBuffer->Name = String(name.begin(), name.end());
	constantBuffer->Index = reflectedResourceDesc.BindPoint;
	if(Contains(constantBuffer->Name, DT_TEXT("PerFrame"), false))
	{
		constantBuffer->BufferType = ConstantBufferType::PerFrame;
	}
	else if(Contains(constantBuffer->Name, DT_TEXT("PerObject"), false))
	{
		constantBuffer->BufferType = ConstantBufferType::PerObject;
	}
	else
	{
		constantBuffer->BufferType = ConstantBufferType::PerDrawCall;
	}

	// Create variables that given constant buffer contains
	for(unsigned int j = 0; j < reflectedConstantBufferDesc.Variables; ++j)
	{
		// Get variable description
		ID3D11ShaderReflectionVariable* reflectedVariable = reflectedConstantBuffer->GetVariableByIndex(j);
		D3D11_SHADER_VARIABLE_DESC reflectedVariableDesc;
		result = reflectedVariable->GetDesc(&reflectedVariableDesc);
		HR_REACTION(result, gDebug.Print(LogVerbosity::Error, CHANNEL_GRAPHICS, DT_TEXT("Cannot obtain variable description!")));

		ID3D11ShaderReflectionType* reflectedVariableType = reflectedVariable->GetType();
		D3D11_SHADER_TYPE_DESC reflectedVariableTypeDesc;
		result = reflectedVariableType->GetDesc(&reflectedVariableTypeDesc);
		HR_REACTION(result, gDebug.Print(LogVerbosity::Error, CHANNEL_GRAPHICS, DT_TEXT("Cannot obtain variable type description!")));

		
		// Create variable
		UniquePtr<ShaderVariable> variable = std::make_unique<ShaderVariable>();
		variable->Offset = reflectedVariableDesc.StartOffset;
		const std::string name = reflectedVariableDesc.Name;
		variable->Name = String(name.begin(), name.end());
		variable->Size = reflectedVariableDesc.Size;
		variable->SetGetterFunctionFromTypeDescription(reflectedVariableTypeDesc);

		// Push variable to array
		constantBuffer->Variables.push_back(std::move(variable));
	}

	// Push constant buffer to array 
	_constantBuffers[constantBuffer->BufferType].push_back(std::move(constantBuffer));

	return true;
}

bool Shader::Load(const String& path)
{
	Asset::Load(path);

	const String vsFileName = path + DT_TEXT("VS.hlsl");
	const String psFileName = path + DT_TEXT("PS.hlsl");
	Graphics& graphics = gGraphics;

	HRESULT result = D3DCompileFromFile(vsFileName.c_str(), nullptr, nullptr, "main", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &_vertexShaderBuffer, nullptr);
	HR(result);

	result = D3DCompileFromFile(psFileName.c_str(), nullptr, nullptr, "main", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &_pixelShaderBuffer, nullptr);
	HR(result);
	
	return true;
}

bool Shader::Initialize()
{
	if(!_vertexShaderBuffer || !_pixelShaderBuffer)
	{
		return false;
	}

	Graphics& graphics = gGraphics;
	
	if(!graphics.CreateVertexShader(_vertexShaderBuffer, &_vertexShader))
	{
		gDebug.Print(LogVerbosity::Error, CHANNEL_GRAPHICS, DT_TEXT("Failed to create vertex shader"));
		return false;
	}
	if(!graphics.CreatePixelShader(_pixelShaderBuffer, &_pixelShader))
	{
		gDebug.Print(LogVerbosity::Error, CHANNEL_GRAPHICS, DT_TEXT("Failed to create pixel shader"));
		return false;
	}

	if(!GatherConstantBuffersInfo(_vertexShaderBuffer))
	{
		gDebug.Print(LogVerbosity::Error, CHANNEL_GRAPHICS, DT_TEXT("Failed to obtain shader reflection info"));
		return false;
	}

	if(!GatherConstantBuffersInfo(_pixelShaderBuffer))
	{
		gDebug.Print(LogVerbosity::Error, CHANNEL_GRAPHICS, DT_TEXT("Failed to obtain shader reflection info"));
		return false;
	}

	for(auto& constantBufferArray : _constantBuffers)
	{
		for(auto& constantBuffer : constantBufferArray.second)
		{
			if(!constantBuffer->Initialize(graphics))
			{
				gDebug.Printf(LogVerbosity::Error, CHANNEL_GRAPHICS, DT_TEXT("Cannot initialize buffer named %s!"), constantBuffer->Name.c_str());
				return false;
			}
		}
	}

	RELEASE_COM(_pixelShaderBuffer);

	D3D11_INPUT_ELEMENT_DESC inputLayoutDesc[3];
	inputLayoutDesc[0] = {0};
	inputLayoutDesc[1] = {0};
	inputLayoutDesc[2] = {0};

	inputLayoutDesc[0].SemanticName = "POSITION";
	inputLayoutDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputLayoutDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	inputLayoutDesc[1].SemanticName = "NORMAL";
	inputLayoutDesc[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputLayoutDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	inputLayoutDesc[2].SemanticName = "TEXCOORD";
	inputLayoutDesc[2].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputLayoutDesc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	if(!graphics.CreateInputLayout(inputLayoutDesc, sizeof(inputLayoutDesc) / sizeof(D3D11_INPUT_ELEMENT_DESC), _vertexShaderBuffer->GetBufferPointer(), (size_t)_vertexShaderBuffer->GetBufferSize(), &_inputLayout))
	{
		gDebug.Print(LogVerbosity::Error, CHANNEL_GRAPHICS, DT_TEXT("Failed to create input layout"));
		return false;
	}

	RELEASE_COM(_vertexShaderBuffer);

	return true;
}

void Shader::Shutdown()
{
	for(auto& constantBufferArray : _constantBuffers)
	{
		for(auto& constantBuffer : constantBufferArray.second)
		{
			constantBuffer->Shutdown();
		}
	}
	_constantBuffers.clear();
	RELEASE_COM(_inputLayout);
	RELEASE_COM(_pixelShader);
	RELEASE_COM(_vertexShader);
	RELEASE_COM(_vertexShaderBuffer);
	RELEASE_COM(_pixelShaderBuffer);
}

void Shader::UpdatePerFrameBuffers(Graphics& graphics, const MaterialParametersCollection& materialParametersCollection)
{
	const DynamicArray<UniquePtr<ShaderConstantBuffer>>& perFrameBuffers = _constantBuffers[ConstantBufferType::PerFrame];
	for(auto& constantBuffer : perFrameBuffers)
	{
		constantBuffer->Update(graphics, materialParametersCollection);
	}
}

void Shader::UpdatePerObjectBuffers(Graphics& graphics, const MaterialParametersCollection& materialParametersCollection)
{
	const DynamicArray<UniquePtr<ShaderConstantBuffer>>& perObjectBuffers = _constantBuffers[ConstantBufferType::PerObject];
	for(auto& constantBuffer : perObjectBuffers)
	{
		constantBuffer->Update(graphics, materialParametersCollection);
	}
}

void Shader::UpdatePerDrawCallBuffers(Graphics& graphics, const MaterialParametersCollection& materialParametersCollection)
{
	const DynamicArray<UniquePtr<ShaderConstantBuffer>>& perDrawCallBuffers = _constantBuffers[ConstantBufferType::PerDrawCall];
	for(auto& constantBuffer : perDrawCallBuffers)
	{
		constantBuffer->Update(graphics, materialParametersCollection);
	}
}