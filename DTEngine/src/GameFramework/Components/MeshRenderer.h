#pragma once

#include "GameFramework/Component.h"

class MeshBase;
class Material;

class MeshRenderer : public Component
{
protected:
	SharedPtr<MeshBase> _mesh;
	SharedPtr<Material> _material;

public:
	MeshRenderer(SharedPtr<GameObject> owner);
	MeshRenderer(const MeshRenderer& other);
	virtual ~MeshRenderer();

protected:
	virtual SharedPtr<Component> Copy(SharedPtr<GameObject> newOwner) const override;

public:
	virtual void Render(Graphics& graphics) override;

	inline void SetMesh(SharedPtr<MeshBase> mesh) { _mesh = mesh; }
	inline void SetMaterial(SharedPtr<Material> material) { _material = material; }

	inline SharedPtr<Material> GetMaterial() const { return _material; }
	inline SharedPtr<MeshBase> GetMesh() const { return _mesh; }
};