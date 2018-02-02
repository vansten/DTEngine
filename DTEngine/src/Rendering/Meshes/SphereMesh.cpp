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
	const float radius = 0.5f;
	const unsigned int rings = 12;
	const unsigned int sectors = 12;

	const float R = 1.0f / (float)(rings - 1);
	const float S = 1.0f / (float)(sectors - 1);

	_verticesCount = rings * sectors;	
	_indicesCount = rings * sectors * 6;

	VertexType* vertices = new VertexType[_verticesCount];
	unsigned int* indices = new unsigned int[_indicesCount];

	unsigned int i = 0;
	unsigned int j = 0;
	for(unsigned int r = 0; r < rings; ++r)
	{
		const float sinR = sin(XM_PI * r * R);
		const float sinRMinusPiDiv2 = sin(-XM_PIDIV2 + XM_PI * r * R);

		for(unsigned int s = 0; s < sectors; ++s, ++i, j += 6)
		{
			const float y = sinRMinusPiDiv2 * radius;
			const float x = cos(XM_2PI * s * S) * sinR * radius;
			const float z = sin(XM_2PI * s * S) * sinR * radius;

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
