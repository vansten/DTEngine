#pragma once

#include "Rendering/MeshBase.h"

class QuadMesh : public MeshBase
{
public:
	QuadMesh();
	QuadMesh(const QuadMesh& other);
	virtual ~QuadMesh();

	virtual bool Initialize(const String& path) override;
};