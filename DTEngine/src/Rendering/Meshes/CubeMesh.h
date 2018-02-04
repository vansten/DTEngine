#pragma once

#include "Rendering/MeshBase.h"

class CubeMesh final : public MeshBase
{
public:
	CubeMesh();
	CubeMesh(const CubeMesh& other);
	virtual ~CubeMesh();

	virtual bool Initialize() override;
};