#pragma once

#include "Core/Platform.h"

#if DT_RELEASE
	// Need to define NDEBUG if not defined one
	// Because of check in Px*.h files:
		// #if !(defined(NDEBUG) ^ defined(_DEBUG))
		// #error(...)
		// #endif
	// which demands that one of those two symbols must be defined
	// in order to built application
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

public:
	bool Initialize();
	void Shutdown();

	void Simulate(float deltaTime);
};

extern const String CHANNEL_PHYSICS;
extern Physics gPhysics;

#define RELEASE_PHYSX(ptr)	\
if(ptr)						\
{							\
	ptr->release();			\
	ptr = nullptr;			\
}