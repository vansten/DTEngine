#include "TriangleMesh.h"

TriangleMesh::TriangleMesh() : MeshBase()
{

}

TriangleMesh::TriangleMesh(const TriangleMesh& other) : MeshBase(other)
{

}

TriangleMesh::~TriangleMesh()
{

}

bool TriangleMesh::Initialize(const String& path)
{
	MeshBase::Initialize(path);

	_verticesCount = 3;
	_indicesCount = 3;

	VertexType* vertices = new VertexType[_verticesCount];
	uint64* indices = new uint64[_indicesCount];

	vertices[0].Position = XMFLOAT3(-1.0f, -1.0f, 0.0f);
	vertices[0].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertices[0].UV = XMFLOAT2(0.0f, 0.0f);

	vertices[1].Position = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertices[1].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertices[1].UV = XMFLOAT2(0.5f, 1.0f);

	vertices[2].Position = XMFLOAT3(1.0f, -1.0f, 0.0f);
	vertices[2].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertices[2].UV = XMFLOAT2(1.0f, 0.0f);

	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;

	bool result = CreateBuffers(vertices, indices);
	
	delete[] vertices;
	delete[] indices;

	return result;
}
