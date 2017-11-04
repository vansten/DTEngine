#include "CapsuleMesh.h"

CapsuleMesh::CapsuleMesh() : MeshBase()
{

}

CapsuleMesh::CapsuleMesh(const CapsuleMesh& other) : MeshBase(other)
{

}

CapsuleMesh::~CapsuleMesh()
{

}

bool CapsuleMesh::Initialize(const string& path)
{
	MeshBase::Initialize(path);
	
	uint32 numRings = 8;
	uint32 numSegments = 24;
	uint32 numSegmentsHeight = 1;

	const float32 radius = 0.25f;
	const float32 height = 0.5f;

	_verticesCount = (2 * numRings + 2) * (numSegments + 1) + (numSegmentsHeight - 1) * (numSegments + 1);
	_indicesCount = (2 * numRings + 1) * (numSegments + 1) * 6 + (numSegmentsHeight - 1) * (numSegments + 1) * 6;

	VertexType* vertices = new VertexType[_verticesCount];
	uint64* indices = new uint64[_indicesCount];

	const float32 deltaRingAngle = XM_PIDIV2 / numRings;
	const float32 deltaSegAngle = XM_2PI / numSegments;

	const float32 sphereRatio = radius / (2 * radius + height);
	const float32 cylinderRatio = height / (2 * radius + height);

	uint32 index = 0;
	uint32 indicesIndex = 0;

	// Top sphere part
	for(uint32 ring = 0; ring <= numRings; ++ring)
	{
		float32 r0 = radius * sin(ring * deltaRingAngle);
		float32 y0 = radius * cos(ring * deltaRingAngle);

		for(uint32 seg = 0; seg <= numSegments; ++seg)
		{
			const float32 x0 = r0 * cos(seg * deltaSegAngle);
			const float32 z0 = r0 * sin(seg * deltaSegAngle);

			vertices[index].Position = XMFLOAT3(x0, 0.5f * height + y0, z0);

			indices[indicesIndex] = index + numSegments + 1;
			indices[indicesIndex + 1] = index + numSegments;
			indices[indicesIndex + 2] = index;
			indices[indicesIndex + 3] = index + numSegments + 1;
			indices[indicesIndex + 4] = index;
			indices[indicesIndex + 5] = index + 1;

			++index;
			indicesIndex += 6;
		}
	}

	// Cylinder part
	float32 deltaAngle = XM_2PI / numSegments;
	float32 deltaHeight = height / numSegmentsHeight;

	for(uint32 i = 1; i < numSegmentsHeight; ++i)
	{
		for(uint32 j = 0; j <= numSegments; ++j)
		{
			float32 x0 = radius * cos(j * deltaAngle);
			float32 z0 = radius * sin(j * deltaAngle);

			vertices[index].Position = XMFLOAT3(x0, 0.5f * height - i * deltaHeight, z0);

			indices[indicesIndex] = index + numSegments + 1;
			indices[indicesIndex + 1] = index + numSegments;
			indices[indicesIndex + 2] = index;
			indices[indicesIndex + 3] = index + numSegments + 1;
			indices[indicesIndex + 4] = index;
			indices[indicesIndex + 5] = index + 1;

			++index;
			indicesIndex += 6;
		}
	}

	// Bottom sphere part
	for(uint32 ring = 0; ring <= numRings; ++ring)
	{
		float32 r0 = radius * sin(XM_PIDIV2 + ring * deltaRingAngle);
		float32 y0 = radius * cos(XM_PIDIV2 + ring * deltaRingAngle);

		for(uint32 seg = 0; seg <= numSegments; ++seg)
		{
			const float32 x0 = r0 * cos(seg * deltaSegAngle);
			const float32 z0 = r0 * sin(seg * deltaSegAngle);

			vertices[index].Position = XMFLOAT3(x0, -0.5f * height + y0, z0);

			if(ring != numRings)
			{
				indices[indicesIndex] = index + numSegments + 1;
				indices[indicesIndex + 1] = index + numSegments;
				indices[indicesIndex + 2] = index;
				indices[indicesIndex + 3] = index + numSegments + 1;
				indices[indicesIndex + 4] = index;
				indices[indicesIndex + 5] = index + 1;

				indicesIndex += 6;
			}

			++index;
		}
	}

	bool result = CreateBuffers(vertices, indices);

	delete[] vertices;
	delete[] indices;

	return result;
}
