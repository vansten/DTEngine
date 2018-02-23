#include "QuadMesh.h"

QuadMesh::QuadMesh() : MeshBase()
{}

QuadMesh::QuadMesh(const QuadMesh& other) : MeshBase(other)
{}

QuadMesh::~QuadMesh()
{}

bool QuadMesh::Initialize()
{
	_verticesCount = 4;
	_indicesCount = 6;

	VertexType* vertices = new VertexType[_verticesCount];
	unsigned int* indices = new unsigned int[_indicesCount];

	vertices[0].Position = Vector3(-0.5f, -0.5f, 0.0f);
	vertices[0].Normal = Vector3(0.0f, 0.0f, -1.0f);
	vertices[0].UV = Vector2(0.0f, 0.0f);

	vertices[1].Position = Vector3(-0.5f, 0.5f, 0.0f);
	vertices[1].Normal = Vector3(0.0f, 0.0f, -1.0f);
	vertices[1].UV = Vector2(0.0f, 1.0f);

	vertices[2].Position = Vector3(0.5f, -0.5f, 0.0f);
	vertices[2].Normal = Vector3(0.0f, 0.0f, -1.0f);
	vertices[2].UV = Vector2(1.0f, 0.0f);

	vertices[3].Position = Vector3(0.5f, 0.5f, 0.0f);
	vertices[3].Normal = Vector3(0.0f, 0.0f, -1.0f);
	vertices[3].UV = Vector2(1.0f, 1.0f);

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
