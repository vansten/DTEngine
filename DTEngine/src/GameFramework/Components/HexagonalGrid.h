#pragma once

#include "GameFramework/GameObject.h"
#include "GameFramework/Component.h"
#include "Utility/Math.h"
#include <unordered_map>

struct CubeCoordinates;

enum class HexagonDirection
{
	NORTH		= 0,
	NORTH_EAST	= 1,
	SOUTH_EAST	= 2,
	SOUTH		= 3,
	SOUTH_WEST	= 4,
	NORTH_WEST	= 5,

	COUNT		= 6
};

// Stores axial coordinates for hexagonal map
struct AxialCoordinates
{
public:
	int32 X;
	int32 Y;

public:
	AxialCoordinates(int32 x = 0, int32 y = 0);
	AxialCoordinates(const AxialCoordinates& other);
	AxialCoordinates(const CubeCoordinates& cubeCoordinates);

	int32 Distance(const AxialCoordinates& other) const;

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
struct AxialCoordinatesHasher
{
	inline std::size_t operator()(const AxialCoordinates& key) const
	{
		return (key.X * 0x1f1f1f1f) ^ key.Y;
	}
};

// Stores cube coordinates for hexagonal map
struct CubeCoordinates
{
public:
	int32 X;
	int32 Y;
	int32 Z;

public:
	CubeCoordinates(int32 x = 0, int32 y = 0, int32 z = 0);
	CubeCoordinates(const CubeCoordinates& other);
	CubeCoordinates(const AxialCoordinates& axialCoordinates);

	int32 Distance(CubeCoordinates& other) const;

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
class Hexagon : public Component
{
protected:
	AxialCoordinates _coordinates;
	GameObject* _objectOnHexagon;

public:
	Hexagon(GameObject* owner);
	Hexagon(const Hexagon& other);
	virtual ~Hexagon();

protected:
	virtual Hexagon* Copy(GameObject* newOwner) const override;

public:
	// Returns distance from this hexagon to the other
	inline int32 Distance(Hexagon* other)
	{
		if(!other)
		{
			return -1;
		}

		return _coordinates.Distance(other->_coordinates);
	}

	inline AxialCoordinates GetCoordinates() const { return _coordinates; }
	inline GameObject* GetObjectOn() const { return _objectOnHexagon; }
	
	inline void SetCoordinates(const AxialCoordinates& coordinates)
	{
		_coordinates = coordinates;
	}

	inline void SetObjectOn(GameObject* object)
	{
		_objectOnHexagon = object;
	}
};

// Struct containing path calculated by HexagonalGrid
// Stores data about path through hexagons as well as world positions path
struct HexagonalGridPath
{
	friend class HexagonalGrid;

protected:
	std::vector<Hexagon*> _path;
	std::vector<XMFLOAT3> _worldPath;

public:
	inline HexagonalGridPath()
	{

	}

protected:
	inline void AddHexagonToPath(Hexagon* hex)
	{
		_path.push_back(hex);
	}

	void ReversePath();
	void ConstructWorldPath();

public:
	// Returns a path constructed from hexagons (i.e. for displaying something fancy on them)
	inline const std::vector<Hexagon*>& GetPath() const { return _path; }

	// Returns a path constructed from real world positions (i.e. for usage in navigation)
	inline const std::vector<XMFLOAT3>& GetWorldPath() const { return _worldPath; }

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
class HexagonalGrid : public Component
{
	friend class HexagonalGridUtility;

public:
	using CanWalkPredicate = bool(*)(Hexagon*);

protected:
	std::unordered_map<const AxialCoordinates, Hexagon*, AxialCoordinatesHasher> _hexagonalMap;
	float32 _hexagonSize;
	uint32 _width;
	uint32 _height;

public:
	HexagonalGrid(GameObject* owner);
	HexagonalGrid(const HexagonalGrid& other);
	virtual ~HexagonalGrid();

protected:
	virtual HexagonalGrid* Copy(GameObject* newOwner) const override;

public:
	// Returns neighboor of given hexagon along given direction (or nullptr if there is no neighboor)
	Hexagon* GetNeighboor(const Hexagon* hexagon, HexagonDirection direction) const;
	// Returns hexagon at given coordinates (or nullptr if there isn't a hexagon with given coordinates)
	Hexagon* GetHexagonAt(const AxialCoordinates& axialCoordinates) const;
	// Returns hexagon at given world position (or nullptr if there isn't a hexagon at given position)
	Hexagon* GetHexagonAt(const XMFLOAT3& worldPosition) const;

	// Returns whether there exists a path between start and target hexagons
	bool CalculatePath(Hexagon* start, Hexagon* target, HexagonalGridPath& outPath, CanWalkPredicate canWalkPredicate = nullptr) const;
};

class HexagonalGridUtility
{
public:
	static AxialCoordinates AxialDirections[(uint64)HexagonDirection::COUNT];

public:
	// Constructs hexagonal grid with given width and height from hexagons with given size (hexagonSize)
	// Attaches created grid to gridOwner object
	// Returns created grid (there is no need to attach this grid to an object after these functions returns)
	// Return nullptr if grid creation wasn't successful (i.e. gridOwner was nullptr, width, height or hexagonSize was less or equal to 0)
	static HexagonalGrid* CreateGrid(uint32 width, uint32 height, float32 hexagonSize, GameObject* gridOwner);
};