#pragma once

#include "ResourceManagement/Asset.h"
#include "Utility/Math.h"
#include "Utility/BoundingBox.h"

struct ID3D11Buffer;

class MeshBase : public Asset
{
public:
	struct VertexType
	{
	public:
		XMFLOAT3 Position;
		XMFLOAT3 Normal;
		XMFLOAT2 UV;
	};

protected:
	ID3D11Buffer* _vertexBuffer;
	ID3D11Buffer* _indexBuffer;
	unsigned int _verticesCount;
	unsigned int _indicesCount;

	BoundingBox _boundingBox;

public:
	MeshBase();
	MeshBase(const MeshBase& other);
	virtual ~MeshBase();

protected:
	bool CreateBuffers(VertexType* vertices, unsigned int* indices);

public:
	virtual void Shutdown() override;

	inline ID3D11Buffer* GetVertexBuffer() const { return _vertexBuffer; }
	inline ID3D11Buffer* GetIndexBuffer() const { return _indexBuffer; }
	inline unsigned int GetIndicesCount() const { return _indicesCount; }
	inline unsigned int GetVertexTypeSize() const { return sizeof(VertexType); }

	inline const BoundingBox& GetBoundingBox() const { return _boundingBox; }
};