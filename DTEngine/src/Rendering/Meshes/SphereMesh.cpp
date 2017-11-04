#include "SphereMesh.h"

SphereMesh::SphereMesh()
{

}

SphereMesh::SphereMesh(const SphereMesh& other)
{

}

SphereMesh::~SphereMesh()
{

}

bool SphereMesh::Initialize(const string& path)
{
	MeshBase::Initialize(path);
	
	const float radius = 0.5f;
	const uint32 rings = 12;
	const uint32 sectors = 12;

	const float R = 1.0f / (float)(rings - 1);
	const float S = 1.0f / (float)(sectors - 1);

	_verticesCount = rings * sectors;	
	_indicesCount = rings * sectors * 6;

	VertexType* vertices = new VertexType[_verticesCount];
	uint64* indices = new uint64[_indicesCount];

	uint32 i = 0;
	uint32 j = 0;
	for(uint32 r = 0; r < rings; ++r)
	{
		for(uint32 s = 0; s < sectors; ++s, ++i, j += 6)
		{
			float y = sin(-XM_PIDIV2 + XM_PI * r * R) * radius;
			float x = cos(XM_2PI * s * S) * sin(XM_PI * r * R) * radius;
			float z = sin(XM_2PI * s * S) * sin(XM_PI * r * R) * radius;

			vertices[i].Position = XMFLOAT3(x, y, z);

			if(r < rings - 1 && s < sectors - 1)
			{
				indices[j] = r * sectors + s;
				indices[j + 1] = (r + 1) * sectors + s;
				indices[j + 2] = r * sectors + (s + 1);
				indices[j + 3] = r * sectors + (s + 1);
				indices[j + 4] = (r + 1) * sectors + s;
				indices[j + 5] = (r + 1) * sectors + (s + 1);
			}
		}
	}

	bool result = CreateBuffers(vertices, indices);

	delete[] vertices;
	delete[] indices;

	return result;
}
