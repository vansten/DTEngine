#include "MeshRenderer.h"

#include "Debug/Debug.h"
#include "GameFramework/Entity.h"
#include "Rendering/MeshBase.h"
#include "Rendering/Material.h"
#include "ResourceManagement/Resources.h"

DynamicArray<SharedPtr<MeshRenderer>> MeshRenderer::_allRenderers;

MeshRenderer::MeshRenderer(SharedPtr<Entity> owner) : Component(owner), _mesh(nullptr), _material(nullptr)
{
	_material = gResources.Get<Material>();
}

MeshRenderer::MeshRenderer(const MeshRenderer& other) : Component(other), _mesh(other._mesh), _material(other._material)
{}

MeshRenderer::~MeshRenderer()
{}

void MeshRenderer::RegisterMeshRenderer(SharedPtr<MeshRenderer> meshRenderer)
{
	auto& found = std::find(_allRenderers.begin(), _allRenderers.end(), meshRenderer);
	if (found != _allRenderers.end())
	{
		gDebug.Printf(LogVerbosity::Error, CHANNEL_GRAPHICS, DT_TEXT("Renderer of object %s already registered!"), meshRenderer->GetOwner()->GetName().c_str());
		return;
	}
	_allRenderers.push_back(meshRenderer);
}

void MeshRenderer::UnregisterMeshRenderer(SharedPtr<MeshRenderer> meshRenderer)
{
	auto& found = std::find(_allRenderers.begin(), _allRenderers.end(), meshRenderer);
	if (found != _allRenderers.end())
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
	if (!_mesh)
	{
		gDebug.Print(LogVerbosity::Warning, CHANNEL_GRAPHICS, DT_TEXT("Trying to render mesh renderer without mesh set. Aborting"));
		return;
	}

	if (!_material)
	{
		gDebug.Print(LogVerbosity::Warning, CHANNEL_GRAPHICS, DT_TEXT("Trying to render mesh renderer without material set. Aborting"));
#if DT_DEBUG
		graphics.SetMaterial(gResources.GetDefaultMaterial());
		graphics.DrawIndexed(_mesh->GetVertexBuffer(), _mesh->GetIndexBuffer(), _mesh->GetIndicesCount(), _mesh->GetVertexTypeSize(), 0);
#endif
		return;
	}

	graphics.SetRenderState(_material->GetRenderState());
	graphics.SetMaterial(_material.get());
	graphics.DrawIndexed(_mesh->GetVertexBuffer(), _mesh->GetIndexBuffer(), _mesh->GetIndicesCount(), _mesh->GetVertexTypeSize(), 0);
}

RenderQueue MeshRenderer::GetQueue() const
{
	if (!_material)
	{
		return RenderQueue::Opaque;
	}

	return _material->GetRenderQueue();
}
