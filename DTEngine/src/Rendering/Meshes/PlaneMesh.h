#pragma once

#include "Rendering/MeshBase.h"

class PlaneMesh : public MeshBase
{
public:
	PlaneMesh();
	PlaneMesh(const PlaneMesh& other);
	virtual ~PlaneMesh();

	virtual bool Initialize() override;
};