#pragma once

#include "Rendering/MeshBase.h"

class QuadMesh final : public MeshBase
{
public:
	QuadMesh();
	QuadMesh(const QuadMesh& other);
	virtual ~QuadMesh();

	virtual bool Initialize() override;
};