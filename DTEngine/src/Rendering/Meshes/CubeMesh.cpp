#include "CubeMesh.h"

CubeMesh::CubeMesh() : MeshBase()
{

}

CubeMesh::CubeMesh(const CubeMesh& other) : MeshBase(other)
{

}

CubeMesh::~CubeMesh()
{

}

bool CubeMesh::Initialize(const string& path)
{
	MeshBase::Initialize(path);

	_verticesCount = 8;
	_indicesCount = 36;

	VertexType* vertices = new VertexType[_verticesCount];
	uint64* indices = new uint64[_indicesCount];

	vertices[0].Position = XMFLOAT3(-0.5f, -0.5f, -0.5f);
	vertices[1].Position = XMFLOAT3(-0.5f, 0.5f, -0.5f);
	vertices[2].Position = XMFLOAT3(0.5f, -0.5f, -0.5f);
	vertices[3].Position = XMFLOAT3(0.5f, 0.5f, -0.5f);

	vertices[4].Position = XMFLOAT3(-0.5f, -0.5f, 0.5f);
	vertices[5].Position = XMFLOAT3(-0.5f, 0.5f, 0.5f);
	vertices[6].Position = XMFLOAT3(0.5f, -0.5f, 0.5f);
	vertices[7].Position = XMFLOAT3(0.5f, 0.5f, 0.5f);

	// Front face
	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	indices[3] = 2;
	indices[4] = 1;
	indices[5] = 3;

	// Right face
	indices[6] = 2;
	indices[7] = 3;
	indices[8] = 6;
	indices[9] = 6;
	indices[10] = 3;
	indices[11] = 7;

	// Back face
	indices[12] = 6;
	indices[13] = 7;
	indices[14] = 4;
	indices[15] = 4;
	indices[16] = 7;
	indices[17] = 5;

	// Left face
	indices[18] = 4;
	indices[19] = 5;
	indices[20] = 0;
	indices[21] = 0;
	indices[22] = 5;
	indices[23] = 1;

	// Top face
	indices[24] = 1;
	indices[25] = 5;
	indices[26] = 3;
	indices[27] = 3;
	indices[28] = 5;
	indices[29] = 7;

	// Bottom face
	indices[30] = 4;
	indices[31] = 0;
	indices[32] = 6;
	indices[33] = 6;
	indices[34] = 0;
	indices[35] = 2;

	bool result = CreateBuffers(vertices, indices);

	delete[] vertices;
	delete[] indices;

	return result;
}
