#include "Scene.h"

#include "Core/Archive.h"
#include "Components/Camera.h"

#include "Components/CameraControl.h"
#include "Components/MeshRenderer.h"
#include "ResourceManagement/ResourceManager.h"
#include "Components/HexagonalGrid.h"

Scene::Scene(const String& scenePath) : _scenePath(scenePath)
{
}

Scene::~Scene()
{

}

void Scene::Load()
{
	Archive archive;	//TODO: Replace with something like FileSystem::GetArchive(_scenePath);
	UNREFERENCED_PARAMETER(archive);

	// TODO: for bytes in archive
	// if bytes tells that it's gameObject
	// GameObject* go = new GameObject(readName);
	// go->Load(archive);

	SharedPtr<GameObject> gridObject = SpawnObject(DT_TEXT("Grid"));
	SharedPtr<GameObject> cameraObject = SpawnObject(DT_TEXT("Camera"));
	SharedPtr<HexagonalGrid> grid = HexagonalGridUtility::CreateGrid(7, 7, 1, gridObject);

	cameraObject->AddComponent<Camera>();
	cameraObject->GetTransform()->SetPosition(XMFLOAT3(0.0f, 10.0f, 0.0f));
	cameraObject->GetTransform()->SetRotation(XMFLOAT3(90.0f, 0.0f, 0.0f));
	cameraObject->AddComponent<CameraControl>();

	SharedPtr<GameObject> testObject = SpawnObject(gridObject, DT_TEXT("Test"));
	testObject->GetTransform()->SetPosition(XMFLOAT3(0.0f, 5.0f, 0.0f));
	
	SharedPtr<Hexagon> h1 = grid->GetHexagonAt(AxialCoordinates(0, 0));
	SharedPtr<Hexagon> h2 = grid->GetHexagonAt(AxialCoordinates(3, 1));
	
	HexagonalGridPath path;
	grid->CalculatePath(h1, h2, path);

	ResourceManager& resourceManager = GetResourceManager();
	
	const DynamicArray<SharedPtr<Hexagon>>& hexagonPath = path.GetPath();
	SharedPtr<Material> greenM = resourceManager.Load<Material>(GREEN_MATERIAL);
	for(auto hexagon : hexagonPath)
	{
		SharedPtr<MeshRenderer> mr = hexagon->GetOwner()->GetComponent<MeshRenderer>();
		if(mr)
		{
			mr->SetMaterial(greenM);
		}
	}

	for(auto go : _gameObjects)
	{
		go->PostLoad();
	}

	if (!Camera::GetMainCamera())
	{
		OutputDebugString(DT_TEXT("\n\nThere is no camera placed on the scene!\n\n"));
	}

	//TODO: Put this something like FileSystem::Close(archive);
}

void Scene::Save()
{
	Archive archive;	//TODO: Replace with something like FileSystem::GetArchive(_scenePath);
	UNREFERENCED_PARAMETER(archive);
	
	for(auto go : _gameObjects)
	{
		go->PreSave();
	}

	// TODO: archive->Clear();
	// foreach GameObject go in _gameObjects
	// go->Save(archive);

	//TODO: Put this something like FileSystem::Close(archive);
}

void Scene::Unload()
{
	for (auto go : _gameObjects)
	{
		go->Shutdown();
	}
	_gameObjects.clear();

	for(auto go : _newGameObjects)
	{
		go->Shutdown();
	}
	_newGameObjects.clear();
}

void Scene::Update(float32 deltaTime)
{
	for (auto go : _newGameObjects)
	{
		_gameObjects.push_back(go);
	}
	_newGameObjects.clear();

	for (auto go : _gameObjects)
	{
		if(go->IsEnabledInHierarchy())
		{
			go->Update(deltaTime);
		}
	}
}

void Scene::Render(Graphics& graphics)
{
	for (auto go : _gameObjects)
	{
		if(go->IsEnabledInHierarchy())
		{
			go->Render(graphics);
		}
	}
}

SharedPtr<GameObject> Scene::SpawnObject(const String& name)
{
	SharedPtr<GameObject> newGO = SharedPtr<GameObject>(new GameObject(name));

	_newGameObjects.push_back(newGO);
	newGO->Initialize();

	return newGO;
}

SharedPtr<GameObject> Scene::SpawnObject(SharedPtr<GameObject> original)
{
	SharedPtr<GameObject> newGO = original->Copy();

	_newGameObjects.push_back(newGO);
	newGO->Initialize();

	return newGO;
}

SharedPtr<GameObject> Scene::SpawnObject(SharedPtr<GameObject> original, const String& name)
{
	SharedPtr<GameObject> newGO = original->Copy();
	newGO->SetName(name);

	_newGameObjects.push_back(newGO);
	newGO->Initialize();

	return newGO;
}
