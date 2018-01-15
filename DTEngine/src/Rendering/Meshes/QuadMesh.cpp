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

bool QuadMesh::Initialize()
{
	_verticesCount = 4;
	_indicesCount = 6;

	VertexType* vertices = new VertexType[_verticesCount];
	uint32* indices = new uint32[_indicesCount];

	vertices[0].Position = XMFLOAT3(-0.5f, -0.5f, 0.0f);
	vertices[0].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertices[0].UV = XMFLOAT2(0.0f, 0.0f);

	vertices[1].Position = XMFLOAT3(-0.5f, 0.5f, 0.0f);
	vertices[1].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertices[1].UV = XMFLOAT2(0.0f, 1.0f);

	vertices[2].Position = XMFLOAT3(0.5f, -0.5f, 0.0f);
	vertices[2].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertices[2].UV = XMFLOAT2(1.0f, 0.0f);

	vertices[3].Position = XMFLOAT3(0.5f, 0.5f, 0.0f);
	vertices[3].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertices[3].UV = XMFLOAT2(1.0f, 1.0f);

	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;

	indices[3] = 2;
	indices[4] = 1;
	indices[5] = 3;

	const bool result = CreateBuffers(vertices, indices);

	delete[] vertices;
	delete[] indices;

	return result;
}
