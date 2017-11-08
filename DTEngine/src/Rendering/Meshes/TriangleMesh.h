#pragma once

#include "Rendering/MeshBase.h"

class TriangleMesh : public MeshBase
{
public:
	TriangleMesh();
	TriangleMesh(const TriangleMesh& other);
	virtual ~TriangleMesh();

	virtual bool Initialize(const String& path) override;
};