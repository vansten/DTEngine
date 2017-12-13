#include "MeshBase.h"

#include "Rendering/Graphics.h"
#include "Debug/Debug.h"

MeshBase::MeshBase() : _vertexBuffer(nullptr), _indexBuffer(nullptr), _verticesCount(0), _indicesCount(0)
{

}

MeshBase::MeshBase(const MeshBase& other) : _vertexBuffer(nullptr), _indexBuffer(nullptr), _verticesCount(0), _indicesCount(0)
{
}

MeshBase::~MeshBase()
{
}

bool MeshBase::CreateBuffers(VertexType* vertices, uint32* indices)
{
	Graphics& graphics = GetGraphics();

	D3D11_BUFFER_DESC bufferDesc = { 0 };
	D3D11_SUBRESOURCE_DATA bufferData = { 0 };

	// Vertex buffer creation
	// Filling desc
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(VertexType) * _verticesCount;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	// Filling data
	bufferData.pSysMem = vertices;

	bool result = graphics.CreateBuffer(bufferDesc, bufferData, &_vertexBuffer);
	if (!result)
	{
		GetDebug().Print(LogVerbosity::Error, CHANNEL_GRAPHICS, DT_TEXT("Failed to create vertex buffer"));
		return false;
	}

	// Index buffer creation
	// Filling desc
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(uint32) * _indicesCount;
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	// Filling data
	bufferData.pSysMem = indices;

	result = graphics.CreateBuffer(bufferDesc, bufferData, &_indexBuffer);
	if(!result)
	{
		GetDebug().Print(LogVerbosity::Error, CHANNEL_GRAPHICS, DT_TEXT("Failed to create index buffer"));
	}

	// Calculate bounding box
	static const auto positionGetter = [](const VertexType& vertex) -> const XMFLOAT3& { return vertex.Position; };
	_boundingBox.CalculateMinMax<VertexType>(vertices, _verticesCount, positionGetter);

	return result;
}

void MeshBase::Shutdown()
{
	RELEASE_COM(_indexBuffer);
	RELEASE_COM(_vertexBuffer);
}