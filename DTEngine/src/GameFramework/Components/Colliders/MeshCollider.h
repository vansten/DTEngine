#pragma once

#include "GameFramework/Components/PhysicalBody.h"
#include "Rendering/MeshBase.h"

class MeshCollider : public Collider
{
private:
	SharedPtr<MeshBase> _mesh;

public:
	inline MeshCollider(SharedPtr<MeshBase> mesh) : Collider(), _mesh(mesh)
	{}
	inline MeshCollider(const MeshCollider& other) : Collider(other), _mesh(other._mesh)
	{}

protected:
	virtual void Load(Archive& archive) override
	{}
	virtual void Initialize() override;

public:
	virtual void MatchToMeshBoundingBox(const BoundingBox& boundingBox) override;
};