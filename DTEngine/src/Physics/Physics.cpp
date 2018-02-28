#include "Physics.h"

#include "Debug/Debug.h"
#include "GameFramework/Components/PhysicalBody.h"
#include "GameFramework/Entity.h"
#include "Rendering/MeshBase.h"

// Those libraries are commented here and not removed
// to provide easy access to a lib when it's required
// by one of PhysX functions. And commented libs are not included
// so the .exe contains only those libs it needs.

#if DT_DEBUG

//#pragma comment(lib, "LowLevelCHECKED.lib")
//#pragma comment(lib, "LowLevelAABBCHECKED.lib")
//#pragma comment(lib, "LowLevelClothCHECKED.lib")
//#pragma comment(lib, "LowLevelDynamicsCHECKED.lib")
//#pragma comment(lib, "LowLevelParticlesCHECKED.lib")
#pragma comment(lib, "PhysX3CHECKED_x64.lib")
//#pragma comment(lib, "PhysX3CharacterKinematicCHECKED_x64.lib")
#pragma comment(lib, "PhysX3CommonCHECKED_x64.lib")
#pragma comment(lib, "PhysX3CookingCHECKED_x64.lib")
#pragma comment(lib, "PhysX3ExtensionsCHECKED.lib")
//#pragma comment(lib, "PhysX3VehicleCHECKED.lib")
#pragma comment(lib, "PxFoundationCHECKED_x64.lib")
#pragma comment(lib, "PxPvdSDKCHECKED_x64.lib")
//#pragma comment(lib, "SceneQueryCHECKED.lib")
//#pragma comment(lib, "SimulationControllerCHECKED.lib")

#elif DT_RELEASE

//#pragma comment(lib, "LowLevel.lib")
//#pragma comment(lib, "LowLevelAABB.lib")2
//#pragma comment(lib, "LowLevelCloth.lib")
//#pragma comment(lib, "LowLevelDynamics.lib")
//#pragma comment(lib, "LowLevelParticles.lib")
#pragma comment(lib, "PhysX3_x64.lib")
//#pragma comment(lib, "PhysX3CharacterKinematic_x64.lib")
#pragma comment(lib, "PhysX3Common_x64.lib")
#pragma comment(lib, "PhysX3Cooking_x64.lib")
#pragma comment(lib, "PhysX3Extensions.lib")
//#pragma comment(lib, "PhysX3Vehicle.lib")
#pragma comment(lib, "PxFoundation_x64.lib")
//#pragma comment(lib, "SceneQuery.lib")
//#pragma comment(lib, "SimulationController.lib")

#endif

const String CHANNEL_PHYSICS = DT_TEXT("Physics");
Physics gPhysics;

using namespace physx;

void PhysicsErrorCallback::reportError(PxErrorCode::Enum code, const char* message, const char* file, int line)
{
	std::string msg = message;
	String msgString(msg.begin(), msg.end());
	std::string fileString = file + std::string(" line: ") + std::to_string(line);
	String fileWString(fileString.begin(), fileString.end());

	switch (code)
	{
		case PxErrorCode::eABORT:
		case PxErrorCode::eINTERNAL_ERROR:
		case PxErrorCode::eINVALID_OPERATION:
		case PxErrorCode::eINVALID_PARAMETER:
		case PxErrorCode::eOUT_OF_MEMORY:
			gDebug.Printf(LogVerbosity::Error, CHANNEL_PHYSICS, DT_TEXT("PhysX Error. Message: %s\n%s"), msgString.c_str(), fileWString.c_str());
			break;
		case PxErrorCode::eDEBUG_WARNING:
		case PxErrorCode::ePERF_WARNING:
			gDebug.Printf(LogVerbosity::Warning, CHANNEL_PHYSICS, DT_TEXT("PhysX Warning. Message: %s\n%s"), msgString.c_str(), fileWString.c_str());
			break;
		case PxErrorCode::eDEBUG_INFO:
			gDebug.Printf(LogVerbosity::Log, CHANNEL_PHYSICS, DT_TEXT("PhysX Log. Message: %s\n%s"), msgString.c_str(), fileWString.c_str());
			break;
	}
}

void* PhysicsAllocator::allocate(size_t size, const char* typeName, const char* filename, int line)
{
	return _aligned_malloc(size, 16);
}

void PhysicsAllocator::deallocate(void* ptr)
{
	_aligned_free(ptr);
}

bool Physics::Initialize()
{
	gDebug.RegisterChannel(CHANNEL_PHYSICS);

	_foundation = PxCreateFoundation(PX_FOUNDATION_VERSION, _allocator, _errorCallback);

	if (!_foundation)
	{
		gDebug.Print(LogVerbosity::Error, CHANNEL_PHYSICS, DT_TEXT("Cannot create physx::PxFoundation instance"));
		return false;
	}

#if DT_DEBUG

	_pvd = PxCreatePvd(*_foundation);
	_pvdTransport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	if (!_pvd || !_pvdTransport || !_pvd->connect(*_pvdTransport, PxPvdInstrumentationFlag::eALL))
	{
		gDebug.Print(LogVerbosity::Warning, CHANNEL_PHYSICS, DT_TEXT("Cannot establish connection with PhysX Visual Debugger"));
		RELEASE_PHYSX(_pvdTransport);
		RELEASE_PHYSX(_pvd);
	}

#else

	_pvdTransport = nullptr;
	_pvd = nullptr;

#endif

	bool trackMemoryAllocations = false;

#if DT_DEBUG

	trackMemoryAllocations = true;

#endif

	PxTolerancesScale scale;
	scale.speed = 9.81f;
	scale.length = 1.0f;
	scale.mass = 1000.0f;

	_physics = PxCreatePhysics(PX_PHYSICS_VERSION, *_foundation, scale, trackMemoryAllocations, _pvd);

	if (!_physics)
	{
		gDebug.Print(LogVerbosity::Error, CHANNEL_PHYSICS, DT_TEXT("Cannot create physx::PxPhysics instance"));
		return false;
	}

	_cpuDispatcher = PxDefaultCpuDispatcherCreate(2);

	if (!_cpuDispatcher)
	{
		gDebug.Print(LogVerbosity::Error, CHANNEL_PHYSICS, DT_TEXT("Cannot create physx::PxCpuDispatcher instance"));
		return false;
	}

	PxSceneDesc sceneDesc(scale);
	sceneDesc.cpuDispatcher = _cpuDispatcher;
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
	sceneDesc.filterShader = PxDefaultSimulationFilterShader;

	_scene = _physics->createScene(sceneDesc);

	if (!_scene)
	{
		gDebug.Print(LogVerbosity::Error, CHANNEL_PHYSICS, DT_TEXT("Cannot create physx::PxScene instance"));
		return false;
	}

	PxCookingParams cookingParams(scale);
	_cooking = PxCreateCooking(PX_PHYSICS_VERSION, *_foundation, cookingParams);
	if (!_cooking)
	{
		gDebug.Print(LogVerbosity::Error, CHANNEL_PHYSICS, DT_TEXT("Cannot create physx::PxCooking instance"));
		return false;
	}

	return true;
}

void Physics::Shutdown()
{
	RELEASE_PHYSX(_cooking);
	RELEASE_PHYSX(_scene);
	if (_cpuDispatcher)
	{
		delete _cpuDispatcher;
		_cpuDispatcher = nullptr;
	}
	RELEASE_PHYSX(_physics);
	RELEASE_PHYSX(_pvd);
	RELEASE_PHYSX(_pvdTransport);
	RELEASE_PHYSX(_foundation);
}

void Physics::PreSimulate(float deltaTime)
{
	// TODO:
	// foreach PhysicalBody pb
	// if (pb.isKinematic())
	//		pb._rigidbody->SetPose(pb.GetOwner().GetPhysXTransform());
}

void Physics::Simulate(float deltaTime)
{
	DT_ASSERT(_scene, DT_TEXT("Cannot advance PhysX simulation without physx::PxScene instance"));
	DT_ASSERT(deltaTime > 0.0f, DT_TEXT("Cannot advance PhysX simulation with deltaTime <= 0"));
	_scene->simulate(deltaTime);
	_scene->fetchResults(true);
}

void Physics::PostSimulate(float deltaTime)
{
	// TODO:
	// foreach PhysicalBody pb
	// pb.GetOwner().SetPhysxTransform(pb._rigidbody->getPose());
}

void Physics::ResolveCallbacks()
{
	// TODO:
	// foreach CollisionInfo ci
	// ci.Report();
	// foreach TriggerInfo ti
	// ti.Report();
}

void Physics::Update(float deltaTime)
{
	PreSimulate(deltaTime);
	Simulate(deltaTime);
	PostSimulate(deltaTime);

	ResolveCallbacks();
}

bool Physics::CreateRigidbody(bool dynamic, float mass, PhysicalBody* physicalBody, PxRigidActor** rigidbody)
{
	DT_ASSERT(physicalBody && rigidbody, DT_TEXT("Cannot create rigidbody either for null physical body or null rigidbody"));

	if (dynamic)
	{
		(*rigidbody) = _physics->createRigidDynamic(physicalBody->GetOwner()->GetTransform());
		PxRigidBody* rb = (*rigidbody)->is<PxRigidBody>();
		if (rb)
		{
			rb->setMass(mass);
		}
	}
	else
	{
		(*rigidbody) = _physics->createRigidStatic(physicalBody->GetOwner()->GetTransform());
	}

	(*rigidbody)->userData = physicalBody;

	return (*rigidbody) != nullptr;
}

bool Physics::CreateMeshShape(SharedPtr<MeshBase> mesh, physx::PxShape** shape)
{
	DT_ASSERT(mesh && shape, DT_TEXT("Cannot create mesh shape either for null mesh or null shape"));
	
	const DynamicArray<MeshBase::VertexType>& vertices = mesh->GetVertices();
	DynamicArray<Vector3> verticesPositions;

	for (unsigned int i = 0; i < vertices.size(); ++i)
	{
		verticesPositions.push_back(vertices[i].Position);
	}

	PxConvexMeshDesc convexDesc;
	convexDesc.points.count = (PxU32)vertices.size();
	convexDesc.points.stride = sizeof(Vector3);
	convexDesc.points.data = verticesPositions.data();
	convexDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX;

	PxDefaultMemoryOutputStream buf;
	PxConvexMeshCookingResult::Enum result;

	if (!_cooking->cookConvexMesh(convexDesc, buf, &result))
	{
		gDebug.Print(LogVerbosity::Error, CHANNEL_PHYSICS, DT_TEXT("Cannot cook convex mesh!"));
		return false;
	}
	PxDefaultMemoryInputData input(buf.getData(), buf.getSize());
	PxConvexMesh* convexMesh = _physics->createConvexMesh(input);

	(*shape) = _physics->createShape(PxConvexMeshGeometry(convexMesh), *(_physics->createMaterial(0.0f, 0.0f, 0.0f)));

	return (*shape) != nullptr;
}
