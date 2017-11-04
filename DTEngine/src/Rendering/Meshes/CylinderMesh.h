#pragma once

#include "Rendering/MeshBase.h"

class CylinderMesh : public MeshBase
{
public:
	CylinderMesh();
	CylinderMesh(const CylinderMesh& other);
	virtual ~CylinderMesh();

	virtual bool Initialize(const string& path) override;
};