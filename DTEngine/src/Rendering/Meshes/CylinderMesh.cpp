#include "CylinderMesh.h"

CylinderMesh::CylinderMesh() : MeshBase()
{

}

CylinderMesh::CylinderMesh(const CylinderMesh& other) : MeshBase()
{
	
}

CylinderMesh::~CylinderMesh()
{
	
}

bool CylinderMesh::Initialize()
{
	const unsigned int baseTriangles = 30;
	_verticesCount = (baseTriangles + 1) * 2;
	// 3 for each triangle in bottom base, 3 for each triangle in top base, 6 for each side quad
	_indicesCount = baseTriangles * 12;

	VertexType* vertices = new VertexType[_verticesCount];
	unsigned int* indices = new unsigned int[_indicesCount];

	// Bottom center
	vertices[baseTriangles].Position = Vector3(0.0f, 0.0f, 0.0f);
	// Top center
	vertices[baseTriangles * 2 + 1].Position = Vector3(0.0f, 1.0f, 0.0f);

	const float angleDelta = 360.0f / baseTriangles;

	for(unsigned int i = 0; i < baseTriangles; ++i)
	{
		const float angleRad = Math::DEG_TO_RAD * angleDelta * i;
		const float cosAngle = Math::Cos(angleRad) * 0.5f;
		const float sinAngle = Math::Sin(angleRad) * 0.5f;
		vertices[i].Position = Vector3(cosAngle, 0.0f, sinAngle);
		vertices[i].Normal = Vector3(0.0f, -1.0f, 0.0f);
		vertices[i].UV = Vector2((cosAngle + 0.5f) * 0.5f, (sinAngle + 0.5f) * 0.5f);

		vertices[baseTriangles + 1 + i].Position = Vector3(cosAngle, 1.0f, sinAngle);
		vertices[baseTriangles + 1 + i].Normal = Vector3(0.0f, 1.0f, 0.0f);
		vertices[baseTriangles + 1 + i].UV = Vector2(0.5f + (cosAngle + 0.5f) * 0.5f, (sinAngle + 0.5f) * 0.5f);

		// Bottom triangle
		indices[i * 3] = (i + 1) % baseTriangles;
		indices[i * 3 + 1] = baseTriangles;
		indices[i * 3 + 2] = i;

		// Top triangle
		indices[(baseTriangles + i) * 3] = baseTriangles + 1 + i;
		indices[(baseTriangles + i) * 3 + 1] = baseTriangles * 2 + 1;
		indices[(baseTriangles + i) * 3 + 2] = baseTriangles + 1 + ((i + 1) % baseTriangles);

		// Side quad
		// First triangle
		indices[(baseTriangles + i) * 6] = i;
		indices[(baseTriangles + i) * 6 + 1] = baseTriangles + 1 + i;
		indices[(baseTriangles + i) * 6 + 2] = (i + 1) % baseTriangles;
		// Second triangle
		indices[(baseTriangles + i) * 6 + 3] = (i + 1) % baseTriangles;
		indices[(baseTriangles + i) * 6 + 4] = baseTriangles + 1 + i;
		indices[(baseTriangles + i) * 6 + 5] = baseTriangles + 1 + ((i + 1) % baseTriangles);
	}

	const bool result = CreateBuffers(vertices, indices);
	
	delete[] vertices;
	delete[] indices;

	return result;
}
