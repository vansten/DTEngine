#pragma once

#include "Rendering/MeshBase.h"

class TriangleMesh final : public MeshBase
{
public:
	TriangleMesh();
	TriangleMesh(const TriangleMesh& other);
	virtual ~TriangleMesh();

	virtual bool Initialize() override;
};