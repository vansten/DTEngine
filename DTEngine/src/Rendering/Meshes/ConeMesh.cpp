#include "ConeMesh.h"

ConeMesh::ConeMesh() : MeshBase()
{

}

ConeMesh::ConeMesh(const ConeMesh& other) : MeshBase(other)
{

}

ConeMesh::~ConeMesh()
{

}

bool ConeMesh::Initialize(const string& path)
{
	MeshBase::Initialize(path);

	const uint32 baseTriangles = 30;
	_verticesCount = baseTriangles + 2;
	// Each triangle in base is 3 indices plus 3 for connecting edge of the base to top center
	_indicesCount = baseTriangles * 6;

	VertexType* vertices = new VertexType[_verticesCount];
	uint64* indices = new uint64[_indicesCount];

	vertices[baseTriangles].Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	vertices[baseTriangles].Normal = XMFLOAT3(0.0f, -1.0f, 0.0f);
	vertices[baseTriangles].UV = XMFLOAT2(0.0f, 0.0f);

	vertices[baseTriangles + 1].Position = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertices[baseTriangles + 1].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertices[baseTriangles + 1].UV = XMFLOAT2(0.0f, 1.0f);

	const float32 angleDelta = 360.0f / baseTriangles;
	for(uint32 i = 0; i < baseTriangles; ++i)
	{
		float32 angleRad = XMConvertToRadians(angleDelta * i);
		float32 cosAngle = cos(angleRad) * 0.5f;
		float32 sinAngle = sin(angleRad) * 0.5f;
		vertices[i].Position = XMFLOAT3(cosAngle, 0.0f, sinAngle);
		vertices[i].Normal = XMFLOAT3(cosAngle, -1.0f, sinAngle);
		Normalize(vertices[i].Normal);
		vertices[i].UV = XMFLOAT2(cosAngle + 0.5f, sinAngle + 0.5f);

		// Bottom triangle
		indices[i * 3] = (i + 1) % baseTriangles;
		indices[i * 3 + 1] = baseTriangles;
		indices[i * 3 + 2] = i;

		// Top triangle
		indices[(baseTriangles + i) * 3] = i;
		indices[(baseTriangles + i) * 3 + 1] = baseTriangles + 1;
		indices[(baseTriangles + i) * 3 + 2] =(i + 1) % baseTriangles;
	}

	bool result = CreateBuffers(vertices, indices);

	delete[] vertices;
	delete[] indices;

	return result;
}
