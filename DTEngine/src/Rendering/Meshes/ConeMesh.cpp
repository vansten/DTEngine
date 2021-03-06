#include "ConeMesh.h"

ConeMesh::ConeMesh() : MeshBase()
{}

ConeMesh::ConeMesh(const ConeMesh& other) : MeshBase(other)
{}

ConeMesh::~ConeMesh()
{}

bool ConeMesh::Initialize()
{
	const unsigned int baseTriangles = 30;
	_verticesCount = baseTriangles + 2;
	// Each triangle in base is 3 indices plus 3 for connecting edge of the base to top center
	_indicesCount = baseTriangles * 6;

	VertexType* vertices = new VertexType[_verticesCount];
	unsigned int* indices = new unsigned int[_indicesCount];

	vertices[baseTriangles].Position = Vector3(0.0f, 0.0f, 0.0f);
	vertices[baseTriangles].Normal = Vector3(0.0f, -1.0f, 0.0f);
	vertices[baseTriangles].UV = Vector2(0.0f, 0.0f);

	vertices[baseTriangles + 1].Position = Vector3(0.0f, 1.0f, 0.0f);
	vertices[baseTriangles + 1].Normal = Vector3(0.0f, 1.0f, 0.0f);
	vertices[baseTriangles + 1].UV = Vector2(0.0f, 1.0f);

	const float angleDelta = 360.0f / baseTriangles;
	for (unsigned int i = 0; i < baseTriangles; ++i)
	{
		const float angleRad = Math::DEG_TO_RAD * angleDelta * i;
		const float cosAngle = Math::Cos(angleRad) * 0.5f;
		const float sinAngle = Math::Sin(angleRad) * 0.5f;
		vertices[i].Position = Vector3(cosAngle, 0.0f, sinAngle);
		vertices[i].Normal = Vector3(cosAngle, -1.0f, sinAngle).GetNormalized();
		vertices[i].UV = Vector2(cosAngle + 0.5f, sinAngle + 0.5f);

		// Bottom triangle
		indices[i * 3] = (i + 1) % baseTriangles;
		indices[i * 3 + 1] = baseTriangles;
		indices[i * 3 + 2] = i;

		// Top triangle
		indices[(baseTriangles + i) * 3] = i;
		indices[(baseTriangles + i) * 3 + 1] = baseTriangles + 1;
		indices[(baseTriangles + i) * 3 + 2] = (i + 1) % baseTriangles;
	}

	const bool result = CreateBuffers(vertices, indices);

	delete[] vertices;
	delete[] indices;

	return result;
}
