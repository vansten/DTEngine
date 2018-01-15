#include "HexagonMesh.h"

HexagonMesh::HexagonMesh() : MeshBase()
{
}

HexagonMesh::HexagonMesh(const HexagonMesh& other) : MeshBase(other)
{
}

HexagonMesh::~HexagonMesh()
{
}

bool HexagonMesh::Initialize()
{
	_verticesCount = 7;
	_indicesCount = 18;

	VertexType* vertices = new VertexType[_verticesCount];
	uint32* indices = new uint32[_indicesCount];
	
	for (int32 i = 0; i < 6; ++i)
	{
		const float32 angleRad = XMConvertToRadians(60.0f * i);
		vertices[i].Position = XMFLOAT3(cos(angleRad) * 0.5f, 0.0f, sin(angleRad) * 0.5f);
		vertices[i].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
		vertices[i].UV = XMFLOAT2(vertices[i].Position.x + 0.5f, vertices[i].Position.z + 0.5f);
		indices[i * 3] = i;
		indices[i * 3 + 1] = 6;
		indices[i * 3 + 2] = (i + 1) % 6;
	}

	vertices[6].Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	
	const bool result = CreateBuffers(vertices, indices);

	delete[] vertices;
	delete[] indices;

	return result;
}
