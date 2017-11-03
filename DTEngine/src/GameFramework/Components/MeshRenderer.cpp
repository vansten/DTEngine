#include "MeshRenderer.h"

#include "Rendering/Graphics.h"
#include "GameFramework/GameObject.h"
#include "Core/Time.h"
#include "Rendering/MeshBase.h"
#include "Rendering/Material.h"

MeshRenderer::MeshRenderer(GameObject* owner) : Component(owner), _mesh(nullptr), _material(nullptr)
{
}

MeshRenderer::MeshRenderer(const MeshRenderer& other) : Component(other), _mesh(nullptr), _material(nullptr)
{
}

MeshRenderer::~MeshRenderer()
{
}

void MeshRenderer::Render()
{
	assert(gGraphics);
	gGraphics->SetMaterial(_material);
	gGraphics->DrawIndexed(_mesh->GetVertexBuffer(), _mesh->GetIndexBuffer(), _mesh->GetIndicesCount(), _mesh->GetVertexTypeSize(), 0);
}
