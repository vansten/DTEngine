#pragma once

#include "Rendering/MeshBase.h"

class CylinderMesh final : public MeshBase
{
public:
	CylinderMesh();
	CylinderMesh(const CylinderMesh& other);
	virtual ~CylinderMesh();

	virtual bool Initialize() override;
};