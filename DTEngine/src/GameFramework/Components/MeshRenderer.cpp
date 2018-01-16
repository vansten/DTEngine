#include "MeshRenderer.h"

#include "Debug/Debug.h"
#include "GameFramework/Entity.h"
#include "Rendering/MeshBase.h"
#include "Rendering/Material.h"
#include "ResourceManagement/ResourceManager.h"

DynamicArray<SharedPtr<MeshRenderer>> MeshRenderer::_allRenderers;

MeshRenderer::MeshRenderer(SharedPtr<Entity> owner) : Component(owner), _mesh(nullptr), _material(nullptr)
{
	ResourceManager& resourceManager = GetResourceManager();
}

MeshRenderer::MeshRenderer(const MeshRenderer& other) : Component(other), _mesh(other._mesh), _material(other._material)
{
}

MeshRenderer::~MeshRenderer()
{
}

void MeshRenderer::RegisterMeshRenderer(SharedPtr<MeshRenderer> meshRenderer)
{
	auto& found = std::find(_allRenderers.begin(), _allRenderers.end(), meshRenderer);
	if(found != _allRenderers.end())
	{
		GetDebug().Printf(LogVerbosity::Error, CHANNEL_GRAPHICS, DT_TEXT("Renderer of object %s already registered!"), meshRenderer->GetOwner()->GetName().c_str());
		return;
	}
	_allRenderers.push_back(meshRenderer);
}

void MeshRenderer::UnregisterMeshRenderer(SharedPtr<MeshRenderer> meshRenderer)
{
	auto& found = std::find(_allRenderers.begin(), _allRenderers.end(), meshRenderer);
	if(found != _allRenderers.end())
	{
		_allRenderers.erase(found);
	}
}

SharedPtr<Component> MeshRenderer::Copy(SharedPtr<Entity> newOwner) const
{
	SharedPtr<MeshRenderer> copy = SharedPtr<MeshRenderer>(new MeshRenderer(*this));
	copy->_owner = newOwner;
	return StaticPointerCast<Component>(copy);
}

void MeshRenderer::OnInitialize()
{
	Component::OnInitialize();

	RegisterMeshRenderer(SharedFromThis());
}

void MeshRenderer::OnShutdown()
{
	Component::OnShutdown();

	UnregisterMeshRenderer(SharedFromThis());
}

void MeshRenderer::OnRender(Graphics& graphics)
{
	if(!_mesh)
	{
		GetDebug().Print(LogVerbosity::Warning, CHANNEL_GRAPHICS, DT_TEXT("Trying to render mesh renderer without mesh set. Aborting"));
		return;
	}

	if(!_material)
	{
		GetDebug().Print(LogVerbosity::Warning, CHANNEL_GRAPHICS, DT_TEXT("Trying to render mesh renderer without material set. Aborting"));
		return;
	}

	graphics.SetRenderState(_material->GetRenderState());
	graphics.SetMaterial(_material);
	graphics.DrawIndexed(_mesh->GetVertexBuffer(), _mesh->GetIndexBuffer(), _mesh->GetIndicesCount(), _mesh->GetVertexTypeSize(), 0);
}

void MeshRenderer::OnOwnerEnableChanged(bool enabled)
{
	if(enabled)
	{
		RegisterMeshRenderer(SharedFromThis());
	}
	else
	{
		UnregisterMeshRenderer(SharedFromThis());
	}
}

void MeshRenderer::OnEnableChanged(bool enabled)
{
	if(enabled)
	{
		RegisterMeshRenderer(SharedFromThis());
	}
	else
	{
		UnregisterMeshRenderer(SharedFromThis());
	}
}

RenderQueue MeshRenderer::GetQueue() const
{
	if(!_material)
	{
		return RenderQueue::Opaque;
	}

	return _material->GetRenderQueue();
}
