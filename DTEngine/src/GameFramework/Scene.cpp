#include "Scene.h"

#include "Core/Archive.h"
#include "Debug/Debug.h"
#include "Components/Camera.h"

#include "Components/CameraControl.h"
#include "Components/MeshRenderer.h"
#include "Components/HexagonalGrid.h"

#include "Components/PhysicalBody.h"
#include "Components/Colliders/MeshCollider.h"
#include "Components/Colliders/BoxCollider.h"
#include "Components/Colliders/SphereCollider.h"
#include "Components/Colliders/CapsuleCollider.h"

Scene::Scene(const String& scenePath) : _scenePath(scenePath)
{}

Scene::~Scene()
{}

void Scene::Load()
{
	Archive archive;	//TODO: Replace with something like FileSystem::GetArchive(_scenePath);
	UNREFERENCED_PARAMETER(archive);

	// for entities in readBytes
	//		Entity* entity = new Entity(name)
	//		_entities.push_back(entity);
	//		entity->Load(archive)

	for (const auto& e : _entities)
	{
		e->Initialize();
	}

	SharedPtr<Entity> gridEntity = SpawnEntity(DT_TEXT("Grid"));
	SharedPtr<Entity> cameraEntity = SpawnEntity(DT_TEXT("Camera"));

	SharedPtr<HexagonalGrid> grid = HexagonalGridUtility::CreateGrid(10, 10, 3, gridEntity);

	cameraEntity->AddComponent<Camera>();
	cameraEntity->SetPosition(Vector3(0.0f, 10.0f, 0.0f));
	cameraEntity->SetRotation(Rotator(90.0f, 0.0f, 0.0f).ToQuaternion());
	cameraEntity->AddComponent<CameraControl>();

	SharedPtr<Hexagon> h1 = grid->GetHexagonAt(AxialCoordinates(0, 0));
	SharedPtr<Hexagon> h2 = grid->GetHexagonAt(AxialCoordinates(3, 1));

	HexagonalGridPath path;
	grid->CalculatePath(h1, h2, path);

	const DynamicArray<SharedPtr<Hexagon>>& hexagonPath = path.GetPath();
	SharedPtr<Material> materialInstance = nullptr;
	if (hexagonPath.size() > 0)
	{
		materialInstance = hexagonPath[0]->GetOwner()->GetComponent<MeshRenderer>()->GetMaterial()->CreateInstance();
		materialInstance->SetColor(DT_TEXT("Color"), Vector4(0.0f, 1.0f, 0.0f, 1.0f));
	}

	for (auto hexagon : hexagonPath)
	{
		SharedPtr<MeshRenderer> mr = hexagon->GetOwner()->GetComponent<MeshRenderer>();
		if (mr)
		{
			mr->SetMaterial(materialInstance);
		}

		SharedPtr<PhysicalBody> physicalBody = hexagon->GetOwner()->AddComponent<PhysicalBody>();
		UniquePtr<Collider> cc = std::make_unique<CapsuleCollider>();
		UniquePtr<Collider> mc = std::make_unique<MeshCollider>(mr->GetMesh());
		physicalBody->AddCollider(std::move(mc));
		physicalBody->AddCollider(std::move(cc));
	}

	//TODO: Put this something like FileSystem::Close(archive);
}

void Scene::Save()
{
	Archive archive;	//TODO: Replace with something like FileSystem::GetArchive(_scenePath);
	UNREFERENCED_PARAMETER(archive);

	// TODO: archive->Clear();

	for (const auto& e : _entities)
	{
		e->Save(archive);
	}

	//TODO: Put this something like FileSystem::Close(archive);
}

void Scene::Unload()
{
	for (auto go : _entities)
	{
		go->Shutdown();
	}
	_entities.clear();

	for (auto go : _newEntities)
	{
		go->Shutdown();
	}
	_newEntities.clear();
}

void Scene::Update(float deltaTime)
{
	for (auto go : _newEntities)
	{
		_entities.push_back(go);
	}
	_newEntities.clear();

	for (auto go : _entities)
	{
		if (go->IsEnabledInHierarchy())
		{
			go->Update(deltaTime);
		}
	}
}

void Scene::Render(Graphics& graphics)
{
	if (!Camera::GetMainCamera())
	{
		gDebug.Print(LogVerbosity::Error, CHANNEL_CAMERA, DT_TEXT("There is no camera placed on the scene!"));
		return;
	}

	const DynamicArray<SharedPtr<Camera>>& cameras = Camera::GetAllCameras();
	for (auto camera : cameras)
	{
		if (camera)
		{
			camera->Render(graphics, MeshRenderer::GetAllMeshRenderers());
		}
	}

	SharedPtr<Camera> main = Camera::GetMainCamera();
	if (main)
	{
		main->RenderDebug(graphics);
		main->RenderSky(graphics);
	}

	for (auto camera : cameras)
	{
		if (camera)
		{
			// camera->RenderUI();
		}
	}
}

SharedPtr<Entity> Scene::SpawnEntity(const String& name)
{
	SharedPtr<Entity> entity = SharedPtr<Entity>(new Entity(name));

	_newEntities.push_back(entity);
	entity->Initialize();

	return entity;
}

SharedPtr<Entity> Scene::SpawnEntity(SharedPtr<Entity> original)
{
	return SpawnEntity(original, original->GetName() + DT_TEXT(" (copy)"));
}

SharedPtr<Entity> Scene::SpawnEntity(SharedPtr<Entity> original, const String& name)
{
	SharedPtr<Entity> entity = original->Copy();
	entity->SetName(name);

	_newEntities.push_back(entity);
	entity->Initialize();

	return entity;
}
