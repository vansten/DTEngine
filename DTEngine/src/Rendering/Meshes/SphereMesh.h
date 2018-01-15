#pragma once

#include "Rendering/MeshBase.h"

class SphereMesh : public MeshBase
{
public:
	SphereMesh();
	SphereMesh(const SphereMesh& other);
	virtual ~SphereMesh();

	virtual bool Initialize() override;
};