#include "HexagonMesh.h"

HexagonMesh::HexagonMesh() : MeshBase()
{}

HexagonMesh::HexagonMesh(const HexagonMesh& other) : MeshBase(other)
{}

HexagonMesh::~HexagonMesh()
{}

bool HexagonMesh::Initialize()
{
	_verticesCount = 7;
	_indicesCount = 18;

	VertexType* vertices = new VertexType[_verticesCount];
	unsigned int* indices = new unsigned int[_indicesCount];

	for (int i = 0; i < 6; ++i)
	{
		const float angleRad = Math::DEG_TO_RAD * 60.0f * i;
		vertices[i].Position = Vector3(Math::Cos(angleRad) * 0.5f, 0.0f, Math::Sin(angleRad) * 0.5f);
		vertices[i].Normal = Vector3(0.0f, 1.0f, 0.0f);
		vertices[i].UV = Vector2(vertices[i].Position.X + 0.5f, vertices[i].Position.Z + 0.5f);
		indices[i * 3] = i;
		indices[i * 3 + 1] = 6;
		indices[i * 3 + 2] = (i + 1) % 6;
	}

	vertices[6].Position = Vector3(0.0f, 0.0f, 0.0f);

	const bool result = CreateBuffers(vertices, indices);

	delete[] vertices;
	delete[] indices;

	return result;
}
