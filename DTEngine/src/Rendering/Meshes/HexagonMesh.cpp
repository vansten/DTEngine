#include "HexagonMesh.h"

#include "Rendering/Graphics.h"

HexagonMesh::HexagonMesh()
{
}

HexagonMesh::HexagonMesh(const HexagonMesh& other)
{
}

HexagonMesh::~HexagonMesh()
{
}

bool HexagonMesh::Initialize(const string& path)
{
	MeshBase::Initialize(path);

	assert(gGraphics);

	_verticesCount = 7;
	_indicesCount = 18;

	VertexType* vertices = new VertexType[_verticesCount];
	uint64* indices = new uint64[_indicesCount];
	
	float32 angleRad = 0.0f;
	for (int32 i = 0; i < 6; ++i)
	{
		angleRad = XMConvertToRadians(60.0f * i);
		vertices[i].Position = XMFLOAT3(cos(angleRad), 0.0f, sin(angleRad));
		indices[i * 3] = i;
		indices[i * 3 + 1] = 6;
		indices[i * 3 + 2] = (i + 1) % 6;
	}

	vertices[6].Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	
	bool result = CreateBuffers(vertices, indices);

	delete[] vertices;
	delete[] indices;

	return result;
}
