#include "SphereMesh.h"

SphereMesh::SphereMesh() : MeshBase()
{

}

SphereMesh::SphereMesh(const SphereMesh& other) : MeshBase(other)
{

}

SphereMesh::~SphereMesh()
{

}

bool SphereMesh::Initialize()
{
	const float32 radius = 0.5f;
	const uint32 rings = 12;
	const uint32 sectors = 12;

	const float32 R = 1.0f / (float32)(rings - 1);
	const float32 S = 1.0f / (float32)(sectors - 1);

	_verticesCount = rings * sectors;	
	_indicesCount = rings * sectors * 6;

	VertexType* vertices = new VertexType[_verticesCount];
	uint32* indices = new uint32[_indicesCount];

	uint32 i = 0;
	uint32 j = 0;
	for(uint32 r = 0; r < rings; ++r)
	{
		const float32 sinR = sin(XM_PI * r * R);
		const float32 sinRMinusPiDiv2 = sin(-XM_PIDIV2 + XM_PI * r * R);

		for(uint32 s = 0; s < sectors; ++s, ++i, j += 6)
		{
			const float32 y = sinRMinusPiDiv2 * radius;
			const float32 x = cos(XM_2PI * s * S) * sinR * radius;
			const float32 z = sin(XM_2PI * s * S) * sinR * radius;

			vertices[i].Position = XMFLOAT3(x, y, z);
			vertices[i].Normal = XMFLOAT3(x, y, z);
			XMVECTOR normal = XMVector3Normalize(XMLoadFloat3(&vertices[i].Normal));
			XMStoreFloat3(&vertices[i].Normal, normal);
			vertices[i].UV = XMFLOAT2(s * S, 1.0f - r * R);

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

	const bool result = CreateBuffers(vertices, indices);

	delete[] vertices;
	delete[] indices;

	return result;
}
