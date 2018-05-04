#pragma once

#include "Core/Platform.h"
#include "Utility/Math.h"

#if DT_RELEASE
// Need to define NDEBUG if not defined one
// Because of check in Px*.h files:
	// #if !(defined(NDEBUG) ^ defined(_DEBUG))
	// #error(...)
	// #endif
// which demands that one of those two symbols must be defined
// in order to build application
#if !defined(NDEBUG)
#define NDEBUG
#define NDEBUG_DEFINED_HERE
#endif
#endif

#include "PhysX/PxPhysicsAPI.h"

#if defined(NDEBUG_DEFINED_HERE)
#undef NDEBUG
#undef NDEBUG_DEFINED_HERE
#endif

class PhysicalBody;
class MeshBase;

struct TriggerInfo
{

};

struct CollisionInfo
{

};

class PhysicsErrorCallback : public physx::PxErrorCallback
{
	virtual void reportError(physx::PxErrorCode::Enum code, const char* message, const char* file, int line);
};

class PhysicsAllocator : public physx::PxAllocatorCallback
{
	virtual void* allocate(size_t size, const char* typeName, const char* filename, int line);
	virtual void deallocate(void* ptr);
};

class Physics final
{
private:
	PhysicsErrorCallback _errorCallback;
	PhysicsAllocator _allocator;

	physx::PxFoundation* _foundation;
	physx::PxPhysics* _physics;

	physx::PxCpuDispatcher* _cpuDispatcher;

	physx::PxScene* _scene;

	physx::PxPvd* _pvd;
	physx::PxPvdTransport* _pvdTransport;

	physx::PxCooking* _cooking;

private:
	void PreSimulate(float deltaTime);
	void Simulate(float deltaTime);
	void PostSimulate(float deltaTime);
	void ResolveCallbacks();

public:
	bool Initialize();
	void Shutdown();

	void Update(float deltaTime);

	bool CreateRigidbody(bool dynamic, float mass, PhysicalBody* physicalBody, physx::PxRigidActor** rigidbody);
	// Fills given PxShape with PxTriangleMeshGeometry based on given mesh
	bool CreateMeshShape(SharedPtr<MeshBase> mesh, physx::PxShape** shape);
	bool CreateBoxShape(const Vector3& halfExtents, physx::PxShape** shape);
	bool CreateSphereShape(float radius, physx::PxShape** shape);
	bool CreateCapsuleShape(float radius, float halfHeight, physx::PxShape** shape);
};

extern const String CHANNEL_PHYSICS;
extern Physics gPhysics;

inline physx::PxVec3 ToPxVec3(const Vector3& v)
{
	return *(reinterpret_cast<const physx::PxVec3*>(&v));
}

inline physx::PxQuat ToPxQuat(const Quaternion& q)
{
	return *(reinterpret_cast<const physx::PxQuat*>(&q));
}

#define RELEASE_PHYSX(ptr)	\
if(ptr)						\
{							\
	ptr->release();			\
	ptr = nullptr;			\
}