#include "MeshBase.h"

#include "Rendering/Graphics.h"

MeshBase::MeshBase() : _vertexBuffer(nullptr), _indexBuffer(nullptr), _verticesCount(0), _indicesCount(0)
{

}

MeshBase::MeshBase(const MeshBase& other) : _vertexBuffer(nullptr), _indexBuffer(nullptr), _verticesCount(0), _indicesCount(0)
{
}

MeshBase::~MeshBase()
{
}

bool MeshBase::CreateBuffers(VertexType* vertices, uint64* indices)
{
	D3D11_BUFFER_DESC bufferDesc = { 0 };
	D3D11_SUBRESOURCE_DATA bufferData = { 0 };

	// Vertex buffer creation
	// Filling desc
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(VertexType) * _verticesCount;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	// Filling data
	bufferData.pSysMem = vertices;

	bool result = gGraphics->CreateBuffer(bufferDesc, bufferData, &_vertexBuffer);
	if (!result)
	{
		return false;
	}

	// Index buffer creation
	// Filling desc
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(uint64) * _indicesCount;
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	// Filling data
	bufferData.pSysMem = indices;

	result = gGraphics->CreateBuffer(bufferDesc, bufferData, &_indexBuffer);

	return result;
}

void MeshBase::Shutdown()
{
	RELEASE_COM(_indexBuffer);
	RELEASE_COM(_vertexBuffer);
}