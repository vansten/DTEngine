#pragma once

#include "GameFramework/Components/PhysicalBody.h"

class BoxCollider : public Collider
{
private:
	Vector3 _size;

public:
	inline BoxCollider() : Collider(), _size(Vector3::ONE)
	{}
	inline BoxCollider(const Vector3& size) : Collider(), _size(size)
	{}
	inline BoxCollider(const BoxCollider& other) : Collider(other), _size(other._size)
	{}

protected:
	virtual void Load(Archive& archive) override
	{}
	virtual void Initialize(PhysicalBody* physicalBody) override;

	virtual void SetScale(const Vector3& scale) override;

public:
	virtual void MatchToMeshBoundingBox(const BoundingBox& boundingBox) override;
	void SetSize(const Vector3& newSize);
};