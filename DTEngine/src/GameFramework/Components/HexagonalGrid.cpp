#include "HexagonalGrid.h"

#include "GameFramework/Entity.h"
#include "GameFramework/Game.h"
#include "ResourceManagement/Resources.h"
#include "MeshRenderer.h"

AxialCoordinates HexagonalGridUtility::AxialDirections[(size_t)HexagonDirection::_COUNT]
{
	AxialCoordinates(0, 1),
	AxialCoordinates(1, 0),
	AxialCoordinates(1, -1),
	AxialCoordinates(0, -1),
	AxialCoordinates(-1, 1),
	AxialCoordinates(-1, 0)
};

AxialCoordinates::AxialCoordinates(int x, int y) : X(x), Y(y)
{

}

AxialCoordinates::AxialCoordinates(const AxialCoordinates& other) : X(other.X), Y(other.Y)
{

}

AxialCoordinates::AxialCoordinates(const CubeCoordinates& cubeCoordinates) : X(cubeCoordinates.X), Y(cubeCoordinates.Z)
{

}

int AxialCoordinates::Distance(const AxialCoordinates& other) const
{
	// Calculate distance using cube coordinates
	return CubeCoordinates(*this).Distance(CubeCoordinates(other));
}

CubeCoordinates::CubeCoordinates(int x, int y, int z) : X(x), Y(y), Z(z)
{

}

CubeCoordinates::CubeCoordinates(const CubeCoordinates& other) : X(other.X), Y(other.Y), Z(other.Z)
{

}

CubeCoordinates::CubeCoordinates(const AxialCoordinates& axialCoordinates) : X(axialCoordinates.X), Z(axialCoordinates.Y), Y(-axialCoordinates.X - axialCoordinates.Y)
{

}

int CubeCoordinates::Distance(CubeCoordinates& other) const
{
	// Calculate distance as half of the differences sum
	return (abs(X - other.X) + abs(Y - other.Y) + abs(Z - other.Z)) / 2;
}

Hexagon::Hexagon(SharedPtr<Entity> owner) : Component(owner), _entityOnHexagon(nullptr), _coordinates(0, 0)
{

}

Hexagon::Hexagon(const Hexagon& other) : Component(other), _entityOnHexagon(nullptr), _coordinates(other._coordinates)
{

}

Hexagon::~Hexagon()
{

}

SharedPtr<Component> Hexagon::Copy(SharedPtr<Entity> newOwner) const
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
		_worldPath.push_back(hexagon->GetOwner()->GetPosition());
	}
}

HexagonalGrid::HexagonalGrid(SharedPtr<Entity> owner) : Component(owner)
{

}

HexagonalGrid::HexagonalGrid(const HexagonalGrid& other) : Component(other), _width(other._width), _height(other._height), _hexagonSize(other._hexagonSize)
{

}

HexagonalGrid::~HexagonalGrid()
{

}

SharedPtr<Component> HexagonalGrid::Copy(SharedPtr<Entity> newOwner) const
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
	if (direction == HexagonDirection::_COUNT || hexagon == nullptr)
	{
		gDebug.Print(LogVerbosity::Warning, CHANNEL_GENERAL, DT_TEXT("Cannot get neighboor. Either direction is invalid or hexagon argument is nullptr"));
		return nullptr;
	}

	// Get axial coordinates of the possible neighboor as hexagon coordinates plus direction
	AxialCoordinates neighboorCoordinates = hexagon->GetCoordinates() + HexagonalGridUtility::AxialDirections[(size_t)direction];
	return GetHexagonAt(neighboorCoordinates);
}

SharedPtr<Hexagon> HexagonalGrid::GetHexagonAt(const AxialCoordinates& axialCoordinates) const
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

SharedPtr<Hexagon> HexagonalGrid::GetHexagonAt(const XMFLOAT3& worldPosition) const
{
	const float hexagonWidth = 2.0f * _hexagonSize;
	const float hexagonHeight = sqrt(3.0f) * _hexagonSize;

	const XMFLOAT3 xDirection(0.75f * hexagonWidth, 0.0f, 0.5f * hexagonHeight);
	const XMFLOAT3 yDirection(0.0f, 0.0f, hexagonHeight);
	
	// Calculate axial coordinates using axial x and y direction and world position of a hexagon
	const int w = (int)round(worldPosition.x / xDirection.x);
	const int h = (int)round((worldPosition.z - xDirection.z * worldPosition.x / xDirection.x) / yDirection.z);

	// Return hexagon at calculated axial coordinates
	return GetHexagonAt(AxialCoordinates(w, h));
}

// Declared and defined here (in cpp) cause it's a helper struct used only to manage A* in pathfinding
struct HexagonalPathNode
{
protected:
	std::shared_ptr<Hexagon> _hexagon;
	int _cost;

public:
	HexagonalPathNode(std::shared_ptr<Hexagon> hex, int cost) : _hexagon(hex), _cost(cost) { }

	inline std::shared_ptr<Hexagon> GetHexagon() const { return _hexagon; }
	inline int GetCost() const { return _cost; }

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
		gDebug.Print(LogVerbosity::Error, CHANNEL_GENERAL, DT_TEXT("CalculatePath failed. Reason: either start or target doesn't exist"));
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
		for(int i = 0; i < (int)HexagonDirection::_COUNT; ++i)
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

SharedPtr<HexagonalGrid> HexagonalGridUtility::CreateGrid(unsigned int width, unsigned int height, float hexagonSize, SharedPtr<Entity> gridOwner)
{
	// If grid owner doesn't exist or width, height or hexagonSize are less or equal than 0
	if (!gridOwner || width <= 0 || height <= 0 || hexagonSize <= 0.0f)
	{
		gDebug.Print(LogVerbosity::Error, CHANNEL_GENERAL, DT_TEXT("Cannot create grid. Reason: either gridOwner is nullptr or width is less than 0 or height is less than 0 or hexagonSize is less than 0"));
		return nullptr;
	}

	SharedPtr<HexagonalGrid> gridComponent = gridOwner->AddComponent<HexagonalGrid>();

	// Calculate half width and half height to properly position hexagons
	const int halfW = (int)(width * 0.5f);
	const int halfH = (int)(height * 0.5f);

	Resources& resourceManager = gResources;
	
	// Load hexagon mesh and default material
	SharedPtr<HexagonMesh> hexagonMesh = resourceManager.Get<HexagonMesh>();

	const float hexagonWidth = 2.0f * hexagonSize * 0.5f;
	const float hexagonHeight = sqrt(3.0f) * hexagonSize * 0.5f;

	const XMFLOAT3 xDirection(0.75f * hexagonWidth, 0.0f, 0.5f * hexagonHeight);
	const XMFLOAT3 yDirection(0.0f, 0.0f, hexagonHeight);

	// Store hexagon size in grid
	gridComponent->_hexagonSize = hexagonSize;
	gridComponent->_width = width;
	gridComponent->_height = height;

	Game& game = GetGame();

	for(int w = 0; w < (int)width; ++w)
	{
		for(int h = 0; h < (int)height; ++h)
		{
			const AxialCoordinates coordinates(w - halfW, h - halfH);

			// Create new game object and mesh renderer for hexagon
			SharedPtr<Entity> hexagonEntity = game.GetActiveScene()->SpawnEntity(DT_TEXT("Hexagon"));
			SharedPtr<MeshRenderer> hexagonRenderer = hexagonEntity->AddComponent<MeshRenderer>();
			// Set visuals
			hexagonRenderer->SetMesh(hexagonMesh);

			// Create new hexagon with coordinates
			SharedPtr<Hexagon> hexagon = hexagonEntity->AddComponent<Hexagon>();
			hexagon->SetCoordinates(coordinates);
			// Sets scale and parent
			hexagonEntity->SetParent(gridOwner);
			hexagonEntity->SetScale(XMFLOAT3(hexagonSize, 1.0f, hexagonSize));

			// Calculate hexagon's position
			XMFLOAT3 position;
			position.x = xDirection.x * coordinates.X;
			position.y = 0.0f;
			position.z = xDirection.z * coordinates.X + yDirection.z * coordinates.Y;
			hexagonEntity->SetPosition(position);

			// Add hexagon to map
			gridComponent->_hexagonalMap.insert({coordinates, hexagon});
		}
	}

	return gridComponent;
}