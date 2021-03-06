#include "MeshBase.h"

#include "Rendering/Graphics.h"
#include "Debug/Debug.h"

MeshBase::MeshBase() : _vertexBuffer(nullptr), _indexBuffer(nullptr), _verticesCount(0), _indicesCount(0)
{}

MeshBase::MeshBase(const MeshBase& other) : _vertexBuffer(nullptr), _indexBuffer(nullptr), _verticesCount(0), _indicesCount(0)
{}

MeshBase::~MeshBase()
{}

bool MeshBase::CreateBuffers(VertexType* vertices, unsigned int* indices)
{
	Graphics& graphics = gGraphics;

	D3D11_BUFFER_DESC bufferDesc = {0};
	D3D11_SUBRESOURCE_DATA bufferData = {0};

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
		gDebug.Print(LogVerbosity::Error, CHANNEL_GRAPHICS, DT_TEXT("Failed to create vertex buffer"));
		return false;
	}

	// Index buffer creation
	// Filling desc
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(unsigned int) * _indicesCount;
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	// Filling data
	bufferData.pSysMem = indices;

	result = graphics.CreateBuffer(bufferDesc, bufferData, &_indexBuffer);
	if (!result)
	{
		gDebug.Print(LogVerbosity::Error, CHANNEL_GRAPHICS, DT_TEXT("Failed to create index buffer"));
	}

	// Calculate bounding box
	static const auto positionGetter = [](const VertexType& vertex) -> const Vector3&{return vertex.Position;};
	_boundingBox.CalculateMinMax<VertexType>(vertices, _verticesCount, positionGetter);

	_vertices.reserve(_verticesCount);
	for (unsigned int i = 0; i < _verticesCount; ++i)
	{
		_vertices.push_back(vertices[i]);
	}

	_indices.reserve(_indicesCount);
	for (unsigned int i = 0; i < _indicesCount; ++i)
	{
		_indices.push_back(indices[i]);
	}

	return result;
}

void MeshBase::Shutdown()
{
	RELEASE_COM(_indexBuffer);
	RELEASE_COM(_vertexBuffer);
}