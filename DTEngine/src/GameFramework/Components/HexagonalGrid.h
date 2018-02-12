#pragma once

#include "GameFramework/Component.h"
#include "Utility/Math.h"

struct CubeCoordinates;

enum class HexagonDirection
{
	NORTH		= 0,
	NORTH_EAST	= 1,
	SOUTH_EAST	= 2,
	SOUTH		= 3,
	SOUTH_WEST	= 4,
	NORTH_WEST	= 5,

	_COUNT		= 6
};

// Stores axial coordinates for hexagonal map
struct AxialCoordinates final
{
public:
	int X;
	int Y;

public:
	AxialCoordinates(int x = 0, int y = 0);
	AxialCoordinates(const AxialCoordinates& other);
	AxialCoordinates(const CubeCoordinates& cubeCoordinates);

	int Distance(const AxialCoordinates& other) const;

	inline bool operator==(const AxialCoordinates& other) const
	{
		return X == other.X && Y == other.Y;
	}

	inline bool operator!=(const AxialCoordinates& other) const
	{
		return X != other.X || Y != other.Y;
	}

	inline AxialCoordinates operator+(const AxialCoordinates& other) const
	{
		return AxialCoordinates(X + other.X, Y + other.Y);
	}

	inline bool operator<(const AxialCoordinates& other) const
	{
		if(X < other.X && Y < other.Y)
		{
			return true;
		}
		return false;
	}
};

// Helper struct for hashing axial coordinates so unordered_map can be used with AxialCoordinates struct
struct AxialCoordinatesHasher final
{
	inline std::size_t operator()(const AxialCoordinates& key) const
	{
		return (key.X * 0x1f1f1f1f) ^ key.Y;
	}
};

// Stores cube coordinates for hexagonal map
struct CubeCoordinates final
{
public:
	int X;
	int Y;
	int Z;

public:
	CubeCoordinates(int x = 0, int y = 0, int z = 0);
	CubeCoordinates(const CubeCoordinates& other);
	CubeCoordinates(const AxialCoordinates& axialCoordinates);

	int Distance(CubeCoordinates& other) const;

	inline bool operator==(const CubeCoordinates& other) const
	{
		return X == other.X && Y == other.Y && Z == other.Z;
	}

	inline bool operator!=(const CubeCoordinates& other) const
	{
		return X != other.X || Y != other.Y || Z != other.Z;
	}
};

// Class representing hexagon
// It's a component so it can be added to a game object
// Can be holding a game object pointer (i.e. building/unit standing on the hexagon)
class Hexagon final : public Component
{
protected:
	AxialCoordinates _coordinates;
	SharedPtr<Entity> _entityOnHexagon;

public:
	Hexagon(SharedPtr<Entity> owner);
	Hexagon(const Hexagon& other);
	virtual ~Hexagon();

	DECLARE_SHARED_FROM_THIS(Hexagon)

protected:
	virtual SharedPtr<Component> Copy(SharedPtr<Entity> newOwner) const override;

public:
	// Returns distance from this hexagon to the other
	inline int Distance(SharedPtr<Hexagon> other)
	{
		if(!other)
		{
			return -1;
		}

		return _coordinates.Distance(other->_coordinates);
	}

	inline AxialCoordinates GetCoordinates() const { return _coordinates; }
	inline SharedPtr<Entity> GetEntityOn() const { return _entityOnHexagon; }
	
	inline void SetCoordinates(const AxialCoordinates& coordinates)
	{
		_coordinates = coordinates;
	}

	inline void SetEntityOn(SharedPtr<Entity> object)
	{
		_entityOnHexagon = object;
	}
};

// Struct containing path calculated by HexagonalGrid
// Stores data about path through hexagons as well as world positions path
struct HexagonalGridPath final
{
	friend class HexagonalGrid;

protected:
	DynamicArray<SharedPtr<Hexagon>> _path;
	DynamicArray<Vector3> _worldPath;

public:
	inline HexagonalGridPath()
	{

	}

protected:
	inline void AddHexagonToPath(SharedPtr<Hexagon> hex)
	{
		_path.push_back(hex);
	}

	void ReversePath();
	void ConstructWorldPath();

public:
	// Returns a path constructed from hexagons (i.e. for displaying something fancy on them)
	inline const DynamicArray<SharedPtr<Hexagon>>& GetPath() const { return _path; }

	// Returns a path constructed from real world positions (i.e. for usage in navigation)
	inline const DynamicArray<Vector3>& GetWorldPath() const { return _worldPath; }

	// Clears path
	inline void ClearPath()
	{
		_path.clear();
		_worldPath.clear();
	}
};

// Component responsible for storing hexagonal grid.
// Provides methods for getting hexagons at some coordinates (both axial and world)
// as well as getting neighboor of a hexagon in given direction
class HexagonalGrid final : public Component
{
	friend class HexagonalGridUtility;

public:
	using CanWalkPredicate = bool(*)(SharedPtr<Hexagon>);

protected:
	Dictionary<const AxialCoordinates, SharedPtr<Hexagon>, AxialCoordinatesHasher> _hexagonalMap;
	float _hexagonSize;
	unsigned int _width;
	unsigned int _height;

public:
	HexagonalGrid(SharedPtr<Entity> owner);
	HexagonalGrid(const HexagonalGrid& other);
	virtual ~HexagonalGrid();

	DECLARE_SHARED_FROM_THIS(HexagonalGrid)

protected:
	virtual SharedPtr<Component> Copy(SharedPtr<Entity> newOwner) const override;

public:
	virtual void OnShutdown() override;

	// Returns neighboor of given hexagon along given direction (or nullptr if there is no neighboor)
	SharedPtr<Hexagon> GetNeighboor(SharedPtr<Hexagon> hexagon, HexagonDirection direction) const;
	// Returns hexagon at given coordinates (or nullptr if there isn't a hexagon with given coordinates)
	SharedPtr<Hexagon> GetHexagonAt(const AxialCoordinates& axialCoordinates) const;
	// Returns hexagon at given world position (or nullptr if there isn't a hexagon at given position)
	SharedPtr<Hexagon> GetHexagonAt(const Vector3& worldPosition) const;

	// Returns whether there exists a path between start and target hexagons
	bool CalculatePath(SharedPtr<Hexagon> start, SharedPtr<Hexagon> target, HexagonalGridPath& outPath, CanWalkPredicate canWalkPredicate = nullptr) const;
};

class HexagonalGridUtility final
{
public:
	static AxialCoordinates AxialDirections[(size_t)HexagonDirection::_COUNT];

public:
	// Constructs hexagonal grid with given width and height from hexagons with given size (hexagonSize)
	// Attaches created grid to gridOwner object
	// Returns created grid (there is no need to attach this grid to an object after these functions returns)
	// Return nullptr if grid creation wasn't successful (i.e. gridOwner was nullptr, width, height or hexagonSize was less or equal to 0)
	static SharedPtr<HexagonalGrid> CreateGrid(unsigned int width, unsigned int height, float hexagonSize, SharedPtr<Entity> gridOwner);
};