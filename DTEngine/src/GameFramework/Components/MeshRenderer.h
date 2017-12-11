#pragma once

#include "GameFramework/Component.h"

class MeshBase;
class Material;

enum class RenderQueue;

class MeshRenderer : public Component
{
protected:
	static DynamicArray<SharedPtr<MeshRenderer>> _allRenderers;

protected:
	SharedPtr<MeshBase> _mesh;
	SharedPtr<Material> _material;

public:
	MeshRenderer(SharedPtr<GameObject> owner);
	MeshRenderer(const MeshRenderer& other);
	virtual ~MeshRenderer();

	DECLARE_SHARED_FROM_THIS(MeshRenderer)

protected:
	virtual SharedPtr<Component> Copy(SharedPtr<GameObject> newOwner) const override;

	static void RegisterMeshRenderer(SharedPtr<MeshRenderer> meshRenderer);
	static void UnregisterMeshRenderer(SharedPtr<MeshRenderer> meshRenderer);

public:
	virtual void Initialize() override;
	virtual void Shutdown() override;
	virtual void OnRender(Graphics& graphics) override;

	virtual void OnEnableChanged(bool enabled) override;

	RenderQueue GetQueue() const;

	inline void SetMesh(SharedPtr<MeshBase> mesh) { _mesh = mesh; }
	inline void SetMaterial(SharedPtr<Material> material) { _material = material; }

	inline SharedPtr<Material> GetMaterial() const { return _material; }
	inline SharedPtr<MeshBase> GetMesh() const { return _mesh; }

	inline static const DynamicArray<SharedPtr<MeshRenderer>>& GetAllMeshRenderers()
	{
		return _allRenderers;
	}
};