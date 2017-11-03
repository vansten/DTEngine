#include "HexagonalGrid.h"

#include <queue>

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

Hexagon::Hexagon(GameObject* owner) : Component(owner), _objectOnHexagon(nullptr), _coordinates(0, 0)
{

}

Hexagon::Hexagon(const Hexagon& other) : Component(other), _objectOnHexagon(nullptr), _coordinates(other._coordinates)
{

}

Hexagon::~Hexagon()
{

}

void HexagonalGridPath::ReversePath()
{
	// Path reversal
	std::vector<Hexagon*> helperPath;
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
	auto helperIt = helperPath.begin();
	auto helperEnd = helperPath.end();
	for(helperIt; helperIt != helperEnd; ++helperIt)
	{
		_path.push_back((*helperIt));
	}
	helperPath.clear();
}

void HexagonalGridPath::ConstructWorldPath()
{
	for(auto it : _path)
	{
		_worldPath.push_back(it->GetOwner()->GetTransform().GetPosition());
	}
}

HexagonalGrid::HexagonalGrid(GameObject* owner) : Component(owner)
{

}

HexagonalGrid::HexagonalGrid(const HexagonalGrid& other) : Component(other)
{

}

HexagonalGrid::~HexagonalGrid()
{

}

Hexagon* HexagonalGrid::GetNeighboor(const Hexagon* hexagon, HexagonDirection direction) const
{
	if (direction == HexagonDirection::COUNT || hexagon == nullptr)
	{
		return nullptr;
	}

	// Get axial coordinates of the possible neighboor as hexagon coordinates plus direction
	AxialCoordinates neighboorCoordinates = hexagon->GetCoordinates() + HexagonalGridUtility::AxialDirections[(uint64)direction];
	return GetHexagonAt(neighboorCoordinates);
}

Hexagon* HexagonalGrid::GetHexagonAt(const AxialCoordinates& axialCoordinates) const
{
	// Try to find hexagon at given coordinates (since coordinates are keys in _hexagonalMap find method can be used)
	// NOTE: No need for [] operator -> it would create an entrance in map if the key wasn't found and this is undesired behaviour in this case
	auto foundIt = _hexagonalMap.find(axialCoordinates);
	if (foundIt != _hexagonalMap.end())
	{
		return (*foundIt).second;
	}

	return nullptr;
}

Hexagon* HexagonalGrid::GetHexagonAt(const XMFLOAT3& worldPosition) const
{
	const float32 hexagonWidth = 2.0f * _hexagonSize;
	const float32 hexagonHeight = sqrt(3.0f) * _hexagonSize;

	const XMFLOAT3 xDirection(0.75f * hexagonWidth, 0.0f, 0.5f * hexagonHeight);
	const XMFLOAT3 yDirection(0.0f, 0.0f, hexagonHeight);
	
	// Calculate axial coordinates using axial x and y direction and world position of a hexagon
	int32 w = (int32)round(worldPosition.x / xDirection.x);
	int32 h = (int32)round((worldPosition.z - xDirection.z * worldPosition.x / xDirection.x) / yDirection.z);

	// Return hexagon at calculated axial coordinates
	return GetHexagonAt(AxialCoordinates(w, h));
}

// Declared and defined here (in cpp) cause it's a helper struct used only to manage A* in pathfinding
struct HexagonalPathNode
{
protected:
	Hexagon* _hexagon;
	int32 _cost;

public:
	HexagonalPathNode(Hexagon* hex, int32 cost) : _hexagon(hex), _cost(cost) { }

	inline Hexagon* GetHexagon() const { return _hexagon; }
	inline int32 GetCost() const { return _cost; }

	inline bool operator>(const HexagonalPathNode& other) const
	{
		return _cost > other._cost;
	}
};

bool HexagonalGrid::CalculatePath(Hexagon* start, Hexagon* target, HexagonalGridPath& outPath, CanWalkPredicate canWalkPredicate) const
{
	// If start or target are nullptr then return false (can't find path when at least one of the path ends doesn't exist)
	if(start == nullptr || target == nullptr)
	{
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
	std::priority_queue<HexagonalPathNode, std::vector<HexagonalPathNode>, std::greater<HexagonalPathNode>> toVisit;
	// Start from start
	toVisit.push(HexagonalPathNode(start, start->Distance(target)));
	// Came from unordered map is used to determine path when target is found by A*
	std::unordered_map<Hexagon*, Hexagon*> cameFrom;
	cameFrom.insert({start, nullptr});

	bool foundTarget = false;
	while(!toVisit.empty() && !foundTarget)
	{
		HexagonalPathNode hexPathNode = toVisit.top();
		Hexagon* hex = hexPathNode.GetHexagon();
		toVisit.pop();
		for(int i = 0; i < (int)HexagonDirection::COUNT; ++i)
		{
			HexagonDirection direction = (HexagonDirection)i;
			Hexagon* neighboor = GetNeighboor(hex, direction);
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
	Hexagon* current = target;
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

HexagonalGrid* HexagonalGridUtility::CreateGrid(uint32 width, uint32 height, float32 hexagonSize, GameObject* gridOwner)
{
	// If grid owner doesn't exist or width, height or hexagonSize are less or equal than 0
	if (!gridOwner || width <= 0 || height <= 0 || hexagonSize <= 0.0f)
	{
		return nullptr;
	}

	HexagonalGrid* gridComponent = gridOwner->AddComponent<HexagonalGrid>();
	Transform& gridTransform = gridOwner->GetTransform();

	// Calculate half width and half height to properly position hexagons
	const int32 halfW = (int32)(width * 0.5f);
	const int32 halfH = (int32)(height * 0.5f);
	
	// Load hexagon mesh and default material
	HexagonMesh* hexagonMesh = gResourceManager->Load<HexagonMesh>(HEXAGON_MESH);
	Material* material = gResourceManager->Load<Material>(RED_MATERIAL);

	const float32 hexagonWidth =  2.0f * hexagonSize;
	const float32 hexagonHeight = sqrt(3.0f) * hexagonSize;

	const XMFLOAT3 xDirection(0.75f * hexagonWidth, 0.0f, 0.5f * hexagonHeight);
	const XMFLOAT3 yDirection(0.0f, 0.0f, hexagonHeight);

	// Store hexagon size in grid
	gridComponent->_hexagonSize = hexagonSize;

	for (int32 w = 0; w < (int32)width; ++w)
	{
		for (int32 h = 0; h < (int32)height; ++h)
		{
			AxialCoordinates coordinates(w - halfW, h - halfH);

			// Create new game object and mesh renderer for hexagon
			GameObject* newHexagonObject = gGame->GetActiveScene()->SpawnObject(DT_TEXT("Hexagon"));
			MeshRenderer* hexagonRenderer = newHexagonObject->AddComponent<MeshRenderer>();
			// Set visuals
			hexagonRenderer->SetMesh(hexagonMesh);
			hexagonRenderer->SetMaterial(material);
			
			// Create new hexagon with coordinates
			Hexagon* hexagon = newHexagonObject->AddComponent<Hexagon>();
			hexagon->SetCoordinates(coordinates);
			// Sets scale and parent
			newHexagonObject->GetTransform().SetParent(&gridTransform);
			newHexagonObject->GetTransform().SetScale(XMFLOAT3(hexagonSize, 1.0f, hexagonSize));
			
			// Calculate hexagon's position
			XMFLOAT3 position;
			position.x = xDirection.x * coordinates.X;
			position.y = 0.0f;
			position.z = xDirection.z * coordinates.X + yDirection.z * coordinates.Y;
			newHexagonObject->GetTransform().SetPosition(position);
			
			// Add hexagon to map
			gridComponent->_hexagonalMap.insert({coordinates, hexagon});
		}
	}

	return gridComponent;
}