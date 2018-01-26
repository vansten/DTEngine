#pragma once

#include "GameFramework/Component.h"
#include "Rendering/Material.h"
#include "Rendering/MeshBase.h"

enum class RenderQueue;

class MeshRenderer : public Component
{
private:
	static DynamicArray<SharedPtr<MeshRenderer>> _allRenderers;

private:
	SharedPtr<MeshBase> _mesh;
	SharedPtr<Material> _material;

public:
	MeshRenderer(SharedPtr<Entity> owner);
	MeshRenderer(const MeshRenderer& other);
	virtual ~MeshRenderer();

	DECLARE_SHARED_FROM_THIS(MeshRenderer)

private:
	static void RegisterMeshRenderer(SharedPtr<MeshRenderer> meshRenderer);
	static void UnregisterMeshRenderer(SharedPtr<MeshRenderer> meshRenderer);

protected:
	virtual SharedPtr<Component> Copy(SharedPtr<Entity> newOwner) const override;

public:
	virtual void OnInitialize() override;
	virtual void OnShutdown() override;
	virtual void OnRender(Graphics& graphics) override;

	virtual void OnOwnerEnableChanged(bool enabled) override;
	virtual void OnEnableChanged(bool enabled) override;

	RenderQueue GetQueue() const;

	inline void SetMesh(SharedPtr<MeshBase> mesh) { _mesh = mesh; }
	inline void SetMaterial(SharedPtr<Material> material) { _material = material; }

	inline SharedPtr<Material> GetMaterial() const { return _material; }
	inline SharedPtr<MeshBase> GetMesh() const { return _mesh; }
	
	inline const BoundingBox& GetBoundingBox() const
	{
		assert(_mesh);
		return _mesh->GetBoundingBox();
	}

	inline static const DynamicArray<SharedPtr<MeshRenderer>>& GetAllMeshRenderers()
	{
		return _allRenderers;
	}
};