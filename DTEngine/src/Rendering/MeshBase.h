#pragma once

#include "ResourceManagement/Asset.h"
#include "Utility/Math.h"

struct ID3D11Buffer;

class MeshBase : public Asset
{
public:
	struct VertexType
	{
	public:
		XMFLOAT3 Position;
	};

protected:
	ID3D11Buffer* _vertexBuffer;
	ID3D11Buffer* _indexBuffer;
	uint32 _verticesCount;
	uint32 _indicesCount;

public:
	MeshBase();
	MeshBase(const MeshBase& other);
	virtual ~MeshBase();

protected:
	bool CreateBuffers(VertexType* vertices, uint64* indices);

public:
	virtual void Shutdown() override;

	inline ID3D11Buffer* GetVertexBuffer() const { return _vertexBuffer; }
	inline ID3D11Buffer* GetIndexBuffer() const { return _indexBuffer; }
	inline uint32 GetIndicesCount() const { return _indicesCount; }
	inline uint32 GetVertexTypeSize() const { return sizeof(VertexType); }
};