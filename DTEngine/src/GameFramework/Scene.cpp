#include "Scene.h"

#include "Core/Archive.h"
#include "Components/Camera.h"

#include "Components/CameraControl.h"
#include "Components/MeshRenderer.h"
#include "ResourceManagement/ResourceManager.h"
#include "Components/HexagonalGrid.h"

Scene::Scene(const string& scenePath) : _scenePath(scenePath)
{
}

Scene::~Scene()
{

}

void Scene::Load()
{
	Archive* archive = new Archive();	//TODO: Replace with something like FileSystem::GetArchive(_scenePath);

	// TODO: for bytes in archive
	// if bytes tells that it's gameObject
	// GameObject* go = new GameObject(readName);
	// go->Load(archive);

	GameObject* gridObject = SpawnObject(DT_TEXT("Grid"));
	GameObject* cameraObject = SpawnObject(DT_TEXT("Camera"));
	HexagonalGrid* grid = HexagonalGridUtility::CreateGrid(7, 7, 1, gridObject);

	cameraObject->AddComponent<Camera>();
	cameraObject->GetTransform().SetPosition(XMFLOAT3(0.0f, 10.0f, 0.0f));
	cameraObject->GetTransform().SetRotation(XMFLOAT3(90.0f, 0.0f, 0.0f));
	cameraObject->AddComponent<CameraControl>();

	Hexagon* h1 = grid->GetHexagonAt(AxialCoordinates(0, 0));
	Hexagon* h2 = grid->GetHexagonAt(AxialCoordinates(3, 1));

	HexagonalGridPath path;
	grid->CalculatePath(h1, h2, path);

	const std::vector<Hexagon*>& hexagonPath = path.GetPath();
	auto pathIt = hexagonPath.begin();
	auto pathEnd = hexagonPath.end();
	Material* greenM = gResourceManager->Load<Material>(GREEN_MATERIAL);
	for(pathIt; pathIt != pathEnd; ++pathIt)
	{
		MeshRenderer* mr = (*pathIt)->GetOwner()->GetComponent<MeshRenderer>();
		if(mr)
		{
			mr->SetMaterial(greenM);
		}
	}

	std::vector<GameObject*>::iterator& it = _gameObjects.begin();
	const std::vector<GameObject*>::iterator& end = _gameObjects.end();
	for (it; it != end; ++it)
	{
		(*it)->PostLoad();
	}

	if (!Camera::GetMainCamera())
	{
		OutputDebugString(DT_TEXT("\n\nThere is no camera placed on the scene!\n\n"));
	}

	delete archive;	//TODO: Replace with something like FileSystem::Close(archive);
}

void Scene::Save()
{
	Archive* archive = new Archive();	//TODO: Replace with something like FileSystem::GetArchive(_scenePath);
	
	std::vector<GameObject*>::iterator& it = _gameObjects.begin();
	const std::vector<GameObject*>::iterator& end = _gameObjects.end();
	for (it; it != end; ++it)
	{
		(*it)->PreSave();
	}

	it = _gameObjects.begin();
	// TODO: archive->Clear();
	// foreach GameObject go in _gameObjects
	// go->Save(archive);

	delete archive;	//TODO: Replace with something like FileSystem::Close(archive);
}

void Scene::Unload()
{
	std::vector<GameObject*>::iterator& it = _gameObjects.begin();
	const std::vector<GameObject*>::iterator& end = _gameObjects.end();
	for (it; it != end; ++it)
	{
		(*it)->Shutdown();
		delete (*it);
		(*it) = 0;
	}
	_gameObjects.clear();
}

void Scene::Update(float32 deltaTime)
{
	std::vector<GameObject*>::iterator& it = _newGameObjects.begin();
	std::vector<GameObject*>::iterator& end = _newGameObjects.end();
	for (it; it != end; ++it)
	{
		_gameObjects.push_back((*it));
	}
	_newGameObjects.clear();

	it = _gameObjects.begin();
	end = _gameObjects.end();
	for (it; it != end; ++it)
	{
		(*it)->Update(deltaTime);
	}
}

void Scene::Render()
{
	std::vector<GameObject*>::iterator& it = _gameObjects.begin();
	std::vector<GameObject*>::iterator& end = _gameObjects.end();
	for (it; it != end; ++it)
	{
		(*it)->Render();
	}
}

GameObject* Scene::SpawnObject(const string& name)
{
	GameObject* newGO = new GameObject(name);

	_newGameObjects.push_back(newGO);
	newGO->Initialize();

	return newGO;
}

GameObject* Scene::SpawnObject(const GameObject& original, const string& name)
{
	GameObject* newGO = new GameObject(original);
	newGO->SetName(name);

	_newGameObjects.push_back(newGO);
	newGO->Initialize();

	return newGO;
}
