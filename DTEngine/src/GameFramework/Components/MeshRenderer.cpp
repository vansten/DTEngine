#include "MeshRenderer.h"

#include "Rendering/Graphics.h"
#include "GameFramework/GameObject.h"
#include "Core/Time.h"
#include "Rendering/MeshBase.h"
#include "Rendering/Material.h"
#include "ResourceManagement/ResourceManager.h"

MeshRenderer::MeshRenderer(GameObject* owner) : Component(owner), _mesh(nullptr), _material(nullptr)
{
	_material = gResourceManager->Load<Material>(MAGENTA_MATERIAL);
}

MeshRenderer::MeshRenderer(const MeshRenderer& other) : Component(other), _mesh(other._mesh), _material(other._material)
{
}

MeshRenderer::~MeshRenderer()
{
}

MeshRenderer* MeshRenderer::Copy(GameObject* newOwner) const
{
	MeshRenderer* copy = new MeshRenderer(*this);
	copy->_owner = newOwner;
	return copy;
}

void MeshRenderer::Render()
{
	assert(gGraphics);

	if(!_mesh)
	{
		OutputDebugString(DT_TEXT("Mesh renderer has unset mesh!\n"));
		return;
	}

	gGraphics->SetMaterial(_material);
	gGraphics->DrawIndexed(_mesh->GetVertexBuffer(), _mesh->GetIndexBuffer(), _mesh->GetIndicesCount(), _mesh->GetVertexTypeSize(), 0);
}
