#pragma once

#include "Rendering/MeshBase.h"

class CubeMesh : public MeshBase
{
public:
	CubeMesh();
	CubeMesh(const CubeMesh& other);
	virtual ~CubeMesh();

	virtual bool Initialize(const String& path) override;
};