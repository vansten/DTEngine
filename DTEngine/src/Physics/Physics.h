#pragma once

#include "PhysX/PxPhysicsAPI.h"

#include "Core/Platform.h"

#if DT_DEBUG

#pragma comment(lib, "PhysX3CommonCHECKED_x64.lib")
#pragma comment(lib, "PhysX3CHECKED_x64.lib")
#pragma comment(lib, "LowLevelAABBCHECKED.lib")
#pragma comment(lib, "LowLevelCHECKED.lib")
#pragma comment(lib, "LowLevelClothCHECKED.lib")
#pragma comment(lib, "LowLevelDynamicsCHECKED.lib")
#pragma comment(lib, "LowLevelParticlesCHECKED.lib")
#pragma comment(lib, "SceneQueryCHECKED.lib")
#pragma comment(lib, "SimulationControllerCHECKED.lib")

#elif DT_RELEASE

#pragma comment(lib, "PhysX3Common_x64.lib")
#pragma comment(lib, "PhysX3_x64.lib")
#pragma comment(lib, "LowLevelAABB.lib")
#pragma comment(lib, "LowLevel.lib")
#pragma comment(lib, "LowLevelCloth.lib")
#pragma comment(lib, "LowLevelDynamics.lib")
#pragma comment(lib, "LowLevelParticles.lib")
#pragma comment(lib, "SceneQuery.lib")
#pragma comment(lib, "SimulationController.lib")

#endif

class Physics
{

};