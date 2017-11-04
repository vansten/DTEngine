#include "QuadMesh.h"

QuadMesh::QuadMesh() : MeshBase()
{

}

QuadMesh::QuadMesh(const QuadMesh& other) : MeshBase(other)
{

}

QuadMesh::~QuadMesh()
{
	
}

bool QuadMesh::Initialize(const string& path)
{
	MeshBase::Initialize(path);

	_verticesCount = 4;
	_indicesCount = 6;

	VertexType* vertices = new VertexType[_verticesCount];
	uint64* indices = new uint64[_indicesCount];

	vertices[0].Position = XMFLOAT3(-0.5f, -0.5f, 0.0f);
	vertices[1].Position = XMFLOAT3(-0.5f, 0.5f, 0.0f);
	vertices[2].Position = XMFLOAT3(0.5f, -0.5f, 0.0f);
	vertices[3].Position = XMFLOAT3(0.5f, 0.5f, 0.0f);

	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;

	indices[3] = 2;
	indices[4] = 1;
	indices[5] = 3;

	bool result = CreateBuffers(vertices, indices);

	delete[] vertices;
	delete[] indices;

	return result;
}
