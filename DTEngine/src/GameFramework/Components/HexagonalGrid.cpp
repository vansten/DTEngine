#include "HexagonalGrid.h"

#include "GameFramework/GameObject.h"
#include "GameFramework/Game.h"
#include "ResourceManagement/ResourceManager.h"
#include "MeshRenderer.h"

AxialCoordinates HexagonalGridUtility::AxialDirections[(uint64)HexagonDirection::COUNT]
{
	AxialCoordinates(0, 1),
	AxialCoordinates(1, 0),
	AxialCoordinates(1, -1),
	AxialCoordinates(0, -1),
	AxialCoordinates(-1, 1),
	AxialCoordinates(-1, 0)
};

AxialCoordinates::AxialCoordinates(int32 x, int32 y) : X(x), Y(y)
{

}

AxialCoordinates::AxialCoordinates(const AxialCoordinates& other) : X(other.X), Y(other.Y)
{

}

AxialCoordinates::AxialCoordinates(const CubeCoordinates& cubeCoordinates) : X(cubeCoordinates.X), Y(cubeCoordinates.Z)
{

}

int32 AxialCoordinates::Distance(const AxialCoordinates& other) const
{
	// Calculate distance using cube coordinates
	return CubeCoordinates(*this).Distance(CubeCoordinates(other));
}

CubeCoordinates::CubeCoordinates(int32 x, int32 y, int32 z) : X(x), Y(y), Z(z)
{

}

CubeCoordinates::CubeCoordinates(const CubeCoordinates& other) : X(other.X), Y(other.Y), Z(other.Z)
{

}

CubeCoordinates::CubeCoordinates(const AxialCoordinates& axialCoordinates) : X(axialCoordinates.X), Z(axialCoordinates.Y), Y(-axialCoordinates.X - axialCoordinates.Y)
{

}

int32 CubeCoordinates::Distance(CubeCoordinates& other) const
{
	// Calculate distance as half of the differences sum
	return (abs(X - other.X) + abs(Y - other.Y) + abs(Z - other.Z)) / 2;
}

Hexagon::Hexagon(SharedPtr<GameObject> owner) : Component(owner), _objectOnHexagon(nullptr), _coordinates(0, 0)
{

}

Hexagon::Hexagon(const Hexagon& other) : Component(other), _objectOnHexagon(nullptr), _coordinates(other._coordinates)
{

}

Hexagon::~Hexagon()
{

}

SharedPtr<Component> Hexagon::Copy(SharedPtr<GameObject> newOwner) const
{
	SharedPtr<Hexagon> copy = SharedPtr<Hexagon>(new Hexagon(*this));
	copy->_owner = newOwner;
	return StaticPointerCast<Component>(copy);
}

void HexagonalGridPath::ReversePath()
{
	// Path reversal
	DynamicArray<SharedPtr<Hexagon>> helperPath;
	// Push all values from original path in reversed order to helperPath
	auto it = _path.rbegin();
	auto end = _path.rend();
	for(it; it != end; ++it)
	{
		helperPath.push_back((*it));
	}

	// Clear original path
	_path.clear();

	// Push back all values from helperPath to path
	for(auto hexagon : helperPath)
	{
		_path.push_back(hexagon);
	}
	helperPath.clear();
}

void HexagonalGridPath::ConstructWorldPath()
{
	for(auto hexagon : _path)
	{
		_worldPath.push_back(hexagon->GetOwner()->GetTransform()->GetPosition());
	}
}

HexagonalGrid::HexagonalGrid(SharedPtr<GameObject> owner) : Component(owner)
{

}

HexagonalGrid::HexagonalGrid(const HexagonalGrid& other) : Component(other), _width(other._width), _height(other._height), _hexagonSize(other._hexagonSize)
{

}

HexagonalGrid::~HexagonalGrid()
{

}

SharedPtr<Component> HexagonalGrid::Copy(SharedPtr<GameObject> newOwner) const
{
	SharedPtr<HexagonalGrid> copy = SharedPtr<HexagonalGrid>(new HexagonalGrid(*this));
	copy->_owner = newOwner;
	return StaticPointerCast<Component>(copy);
}

void HexagonalGrid::OnShutdown()
{
	_hexagonalMap.clear();
}

SharedPtr<Hexagon> HexagonalGrid::GetNeighboor(SharedPtr<Hexagon> hexagon, HexagonDirection direction) const
{
	if (direction == HexagonDirection::COUNT || hexagon == nullptr)
	{
		GetDebug().Print(LogVerbosity::Warning, CHANNEL_GENERAL, DT_TEXT("Cannot get neighboor. Either direction is invalid or hexagon argument is nullptr"));
		return nullptr;
	}

	// Get axial coordinates of the possible neighboor as hexagon coordinates plus direction
	AxialCoordinates neighboorCoordinates = hexagon->GetCoordinates() + HexagonalGridUtility::AxialDirections[(uint64)direction];
	return GetHexagonAt(neighboorCoordinates);
}

SharedPtr<Hexagon> HexagonalGrid::GetHexagonAt(const AxialCoordinates& axialCoordinates) const
{
	// Try to find hexagon at given coordinates (since coordinates are keys in _hexagonalMap find method can be used)
	// NOTE: No need for [] operator -> it would create an entrance in map if the key wasn't found and this is undesired behaviour in this case
	auto foundIt = _hexagonalMap.find(axialCoordinates);
	if (foundIt != _hexagonalMap.end())
	{
		return (*foundIt).second.lock();
	}

	return nullptr;
}

SharedPtr<Hexagon> HexagonalGrid::GetHexagonAt(const XMFLOAT3& worldPosition) const
{
	const float32 hexagonWidth = 2.0f * _hexagonSize;
	const float32 hexagonHeight = sqrt(3.0f) * _hexagonSize;

	const XMFLOAT3 xDirection(0.75f * hexagonWidth, 0.0f, 0.5f * hexagonHeight);
	const XMFLOAT3 yDirection(0.0f, 0.0f, hexagonHeight);
	
	// Calculate axial coordinates using axial x and y direction and world position of a hexagon
	const int32 w = (int32)round(worldPosition.x / xDirection.x);
	const int32 h = (int32)round((worldPosition.z - xDirection.z * worldPosition.x / xDirection.x) / yDirection.z);

	// Return hexagon at calculated axial coordinates
	return GetHexagonAt(AxialCoordinates(w, h));
}

// Declared and defined here (in cpp) cause it's a helper struct used only to manage A* in pathfinding
struct HexagonalPathNode
{
protected:
	std::shared_ptr<Hexagon> _hexagon;
	int32 _cost;

public:
	HexagonalPathNode(std::shared_ptr<Hexagon> hex, int32 cost) : _hexagon(hex), _cost(cost) { }

	inline std::shared_ptr<Hexagon> GetHexagon() const { return _hexagon; }
	inline int32 GetCost() const { return _cost; }

	inline bool operator>(const HexagonalPathNode& other) const
	{
		return _cost > other._cost;
	}
};

bool HexagonalGrid::CalculatePath(SharedPtr<Hexagon> start, SharedPtr<Hexagon> target, HexagonalGridPath& outPath, CanWalkPredicate canWalkPredicate) const
{
	// If start or target are nullptr then return false (can't find path when at least one of the path ends doesn't exist)
	if(start == nullptr || target == nullptr)
	{
		GetDebug().Print(LogVerbosity::Error, CHANNEL_GENERAL, DT_TEXT("CalculatePath failed. Reason: either start or target doesn't exist"));
		return false;
	}

	// If start and target points to the same object or they are at the same location then the path contains only one element
	if(start == target || start->GetCoordinates() == target->GetCoordinates())
	{
		outPath.AddHexagonToPath(start);
		outPath.ConstructWorldPath();
		return true;
	}

	// Priority queue is used for A* algorithm
	PriorityQueue<HexagonalPathNode, DynamicArray<HexagonalPathNode>, Greater<HexagonalPathNode>> toVisit;
	// Start from start
	toVisit.push(HexagonalPathNode(start, start->Distance(target)));
	// Came from unordered map is used to determine path when target is found by A*
	Dictionary<SharedPtr<Hexagon>, SharedPtr<Hexagon>> cameFrom;
	cameFrom.insert({start, nullptr});

	bool foundTarget = false;
	while(!toVisit.empty() && !foundTarget)
	{
		HexagonalPathNode hexPathNode = toVisit.top();
		SharedPtr<Hexagon> hex = hexPathNode.GetHexagon();
		toVisit.pop();
		for(int i = 0; i < (int)HexagonDirection::COUNT; ++i)
		{
			const HexagonDirection direction = (HexagonDirection)i;
			SharedPtr<Hexagon> neighboor = GetNeighboor(hex, direction);
			if(neighboor != nullptr && cameFrom.find(neighboor) == cameFrom.end())
			{
				// If there is no predicate telling whether hexagon is "walkable" or the predicate returns true which means that hexagon is "walkable"
				// Process that hexagon
				if(canWalkPredicate == nullptr || canWalkPredicate(neighboor))
				{
					cameFrom.insert({neighboor, hex});
					if(neighboor == target)
					{
						foundTarget = true;
						break;
					}
					toVisit.push(HexagonalPathNode(neighboor, neighboor->Distance(target)));
				}
			}
		}
	}

	// If target wasn't found then clear path and return false to tell user that target is not reachable from start
	if(!foundTarget)
	{
		outPath.ClearPath();
		return false;
	}

	// Reconstruct path from cameFrom map
	SharedPtr<Hexagon> current = target;
	while(current != nullptr)
	{
		outPath.AddHexagonToPath(current);
		current = cameFrom[current];
	}

	// Reverse path (cause constructing begins from the end of the path(target))
	outPath.ReversePath();
	outPath.ConstructWorldPath();

	return true;
}

SharedPtr<HexagonalGrid> HexagonalGridUtility::CreateGrid(uint32 width, uint32 height, float32 hexagonSize, SharedPtr<GameObject> gridOwner)
{
	// If grid owner doesn't exist or width, height or hexagonSize are less or equal than 0
	if (!gridOwner || width <= 0 || height <= 0 || hexagonSize <= 0.0f)
	{
		GetDebug().Print(LogVerbosity::Error, CHANNEL_GENERAL, DT_TEXT("Cannot create grid. Reason: either gridOwner is nullptr or width is less than 0 or height is less than 0 or hexagonSize is less than 0"));
		return nullptr;
	}

	SharedPtr<HexagonalGrid> gridComponent = gridOwner->AddComponent<HexagonalGrid>();
	SharedPtr<Transform> gridTransform = gridOwner->GetTransform();

	// Calculate half width and half height to properly position hexagons
	const int32 halfW = (int32)(width * 0.5f);
	const int32 halfH = (int32)(height * 0.5f);

	ResourceManager& resourceManager = GetResourceManager();
	
	// Load hexagon mesh and default material
	SharedPtr<HexagonMesh> hexagonMesh = resourceManager.Load<HexagonMesh>(HEXAGON_MESH);
	SharedPtr<Material> material = resourceManager.Load<Material>(WHITE_MATERIAL);

	const float32 hexagonWidth = 2.0f * hexagonSize * 0.5f;
	const float32 hexagonHeight = sqrt(3.0f) * hexagonSize * 0.5f;

	const XMFLOAT3 xDirection(0.75f * hexagonWidth, 0.0f, 0.5f * hexagonHeight);
	const XMFLOAT3 yDirection(0.0f, 0.0f, hexagonHeight);

	// Store hexagon size in grid
	gridComponent->_hexagonSize = hexagonSize;
	gridComponent->_width = width;
	gridComponent->_height = height;

	Game& game = GetGame();

	for(int32 w = 0; w < (int32)width; ++w)
	{
		for(int32 h = 0; h < (int32)height; ++h)
		{
			const AxialCoordinates coordinates(w - halfW, h - halfH);

			// Create new game object and mesh renderer for hexagon
			SharedPtr<GameObject> newHexagonObject = game.GetActiveScene()->SpawnObject(DT_TEXT("Hexagon"));
			SharedPtr<MeshRenderer> hexagonRenderer = newHexagonObject->AddComponent<MeshRenderer>();
			// Set visuals
			hexagonRenderer->SetMesh(hexagonMesh);
			hexagonRenderer->SetMaterial(material);

			// Create new hexagon with coordinates
			SharedPtr<Hexagon> hexagon = newHexagonObject->AddComponent<Hexagon>();
			hexagon->SetCoordinates(coordinates);
			// Sets scale and parent
			newHexagonObject->GetTransform()->SetParent(gridTransform);
			newHexagonObject->GetTransform()->SetScale(XMFLOAT3(hexagonSize, 1.0f, hexagonSize));

			// Calculate hexagon's position
			XMFLOAT3 position;
			position.x = xDirection.x * coordinates.X;
			position.y = 0.0f;
			position.z = xDirection.z * coordinates.X + yDirection.z * coordinates.Y;
			newHexagonObject->GetTransform()->SetPosition(position);

			// Add hexagon to map
			gridComponent->_hexagonalMap.insert({coordinates, hexagon});
		}
	}

	return gridComponent;
}