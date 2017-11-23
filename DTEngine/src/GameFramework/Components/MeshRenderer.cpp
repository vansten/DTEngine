#include "MeshRenderer.h"

#include "Debug/Debug.h"
#include "GameFramework/GameObject.h"
#include "Rendering/MeshBase.h"
#include "Rendering/Material.h"
#include "ResourceManagement/ResourceManager.h"

MeshRenderer::MeshRenderer(SharedPtr<GameObject> owner) : Component(owner), _mesh(nullptr), _material(nullptr)
{
	ResourceManager& resourceManager = GetResourceManager();
	_material = resourceManager.Load<Material>(MAGENTA_MATERIAL);
}

MeshRenderer::MeshRenderer(const MeshRenderer& other) : Component(other), _mesh(other._mesh), _material(other._material)
{
}

MeshRenderer::~MeshRenderer()
{
}

SharedPtr<Component> MeshRenderer::Copy(SharedPtr<GameObject> newOwner) const
{
	SharedPtr<MeshRenderer> copy = SharedPtr<MeshRenderer>(new MeshRenderer(*this));
	copy->_owner = newOwner;
	return StaticPointerCast<Component>(copy);
}

void MeshRenderer::Render(Graphics& graphics)
{
	if(!_mesh)
	{
		GetDebug().Print(LogVerbosity::Warning, CHANNEL_GRAPHICS, DT_TEXT("Trying to render mesh renderer without mesh set. Aborting"));
		return;
	}

	graphics.SetMaterial(_material);
	graphics.DrawIndexed(_mesh->GetVertexBuffer(), _mesh->GetIndexBuffer(), _mesh->GetIndicesCount(), _mesh->GetVertexTypeSize(), 0);
}
