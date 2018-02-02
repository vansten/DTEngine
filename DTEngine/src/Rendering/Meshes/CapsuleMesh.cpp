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

bool CapsuleMesh::Initialize()
{
	const unsigned int numRings = 8;
	const unsigned int numSegments = 24;
	const unsigned int numSegmentsHeight = 1;

	const float radius = 0.25f;
	const float height = 0.5f;

	_verticesCount = (2 * numRings + 2) * (numSegments + 1) + (numSegmentsHeight - 1) * (numSegments + 1);
	_indicesCount = (2 * numRings + 1) * (numSegments + 1) * 6 + (numSegmentsHeight - 1) * (numSegments + 1) * 6;

	VertexType* vertices = new VertexType[_verticesCount];
	unsigned int* indices = new unsigned int[_indicesCount];

	const float deltaRingAngle = XM_PIDIV2 / numRings;
	const float deltaSegAngle = XM_2PI / numSegments;

	const float sphereRatio = radius / (2 * radius + height);
	const float cylinderRatio = height / (2 * radius + height);

	unsigned int index = 0;
	unsigned int indicesIndex = 0;

	// Top sphere part
	for(unsigned int ring = 0; ring <= numRings; ++ring)
	{
		const float r0 = radius * sin(ring * deltaRingAngle);
		const float y0 = radius * cos(ring * deltaRingAngle);

		for(unsigned int seg = 0; seg <= numSegments; ++seg)
		{
			const float x0 = r0 * cos(seg * deltaSegAngle);
			const float z0 = r0 * sin(seg * deltaSegAngle);

			vertices[index].Position = XMFLOAT3(x0, 0.5f * height + y0, z0);
			vertices[index].Normal = XMFLOAT3(x0, y0, z0);
			Normalize(vertices[index].Normal);
			vertices[index].UV = XMFLOAT2((float)seg / numSegments, (float)ring / numRings * sphereRatio);

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
	const float deltaAngle = XM_2PI / numSegments;
	const float deltaHeight = height / numSegmentsHeight;

	for(unsigned int i = 1; i < numSegmentsHeight; ++i)
	{
		for(unsigned int j = 0; j <= numSegments; ++j)
		{
			const float x0 = radius * cos(j * deltaAngle);
			const float z0 = radius * sin(j * deltaAngle);

			vertices[index].Position = XMFLOAT3(x0, 0.5f * height - i * deltaHeight, z0);
			vertices[index].Normal = XMFLOAT3(x0, 0.0f, z0);
			Normalize(vertices[index].Normal);
			vertices[index].UV = XMFLOAT2(j / (float)numSegments, i / (float)numSegmentsHeight * cylinderRatio + sphereRatio);

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
	for(unsigned int ring = 0; ring <= numRings; ++ring)
	{
		const float r0 = radius * sin(XM_PIDIV2 + ring * deltaRingAngle);
		const float y0 = radius * cos(XM_PIDIV2 + ring * deltaRingAngle);

		for(unsigned int seg = 0; seg <= numSegments; ++seg)
		{
			const float x0 = r0 * cos(seg * deltaSegAngle);
			const float z0 = r0 * sin(seg * deltaSegAngle);

			vertices[index].Position = XMFLOAT3(x0, -0.5f * height + y0, z0);
			vertices[index].Normal = XMFLOAT3(x0, y0, z0);
			Normalize(vertices[index].Normal);
			vertices[index].UV = XMFLOAT2((float)seg / numSegments, (float)ring / numRings * sphereRatio + cylinderRatio + sphereRatio);

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
