#include "PlaneMesh.h"

PlaneMesh::PlaneMesh() : MeshBase()
{

}

PlaneMesh::PlaneMesh(const PlaneMesh& other) : MeshBase(other)
{

}

PlaneMesh::~PlaneMesh()
{

}

bool PlaneMesh::Initialize()
{
	const unsigned int size = 16;
	_verticesCount = (size + 1) * (size + 1);
	_indicesCount = size * size * 6;

	VertexType* vertices = new VertexType[_verticesCount];
	unsigned int* indices = new unsigned int[_indicesCount];

	for(unsigned int i = 0, y = 0; y <= size; y++)
	{
		const float yF = -0.5f + (float)y / size;
		for(unsigned int x = 0; x <= size; x++, i++)
		{
			const float xF = -0.5f + (float)x / size;
			vertices[i].Position = XMFLOAT3(xF, yF, 0);
			vertices[i].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
			vertices[i].UV = XMFLOAT2(xF + 0.5f, yF + 0.5f);
		}
	}

	for(unsigned int ti = 0, vi = 0, y = 0; y < size; y++, vi++)
	{
		for(unsigned int x = 0; x < size; x++, ti += 6, vi++)
		{
			indices[ti] = vi; 
			indices[ti + 1] = vi + size + 1;
			indices[ti + 2] = vi + 1;
			indices[ti + 3] = vi + 1;
			indices[ti + 4] = vi + size + 1;
			indices[ti + 5] = vi + size + 2;
		}
	}

	const bool result = CreateBuffers(vertices, indices);

	delete[] vertices;
	delete[] indices;

	return result;
}
