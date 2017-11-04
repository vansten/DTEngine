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

bool PlaneMesh::Initialize(const string& path)
{
	MeshBase::Initialize(path);

	uint32 size = 16;
	_verticesCount = (size + 1) * (size + 1);
	_indicesCount = size * size * 6;

	VertexType* vertices = new VertexType[_verticesCount];
	uint64* indices = new uint64[_indicesCount];

	for(uint32 i = 0, y = 0; y <= size; y++)
	{
		float32 yF = -0.5f + (float32)y / size;
		for(uint32 x = 0; x <= size; x++, i++)
		{
			float32 xF = -0.5f + (float32)x / size;
			vertices[i].Position = XMFLOAT3(xF, yF, 0);
		}
	}

	for(uint32 ti = 0, vi = 0, y = 0; y < size; y++, vi++)
	{
		for(uint32 x = 0; x < size; x++, ti += 6, vi++)
		{
			indices[ti] = vi;
			indices[ti + 3] = indices[ti + 2] = vi + 1;
			indices[ti + 4] = indices[ti + 1] = vi + size + 1;
			indices[ti + 5] = vi + size + 2;
		}
	}

	bool result = CreateBuffers(vertices, indices);

	delete[] vertices;
	delete[] indices;

	return result;
}
