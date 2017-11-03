#pragma once

#include "GameFramework/Component.h"

class MeshBase;
class Material;

class MeshRenderer : public Component
{
protected:
	MeshBase* _mesh;
	Material* _material;

public:
	MeshRenderer(GameObject* owner);
	MeshRenderer(const MeshRenderer& other);
	virtual ~MeshRenderer();

	virtual void Render() override;

	inline void SetMesh(MeshBase* mesh) { _mesh = mesh; }
	inline void SetMaterial(Material* material) { _material = material; }

	inline Material* GetMaterial() const { return _material; }
	inline MeshBase* GetMesh() const { return _mesh; }
};