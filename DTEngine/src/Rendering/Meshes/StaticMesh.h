#pragma once

#include "Rendering/MeshBase.h"

class StaticMesh : public MeshBase
{
public:
	StaticMesh();
	StaticMesh(const StaticMesh& other);
	virtual ~StaticMesh();

protected:
	bool InitializeFromOBJ(const String& path);
	bool InitializeFromFBX(const String& path);

public:
	virtual bool Initialize(const String& path) override;
};