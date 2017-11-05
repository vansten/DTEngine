#pragma once

#include "Rendering/MeshBase.h"

class StaticMesh : public MeshBase
{
public:
	StaticMesh();
	StaticMesh(const StaticMesh& other);
	virtual ~StaticMesh();

protected:
	bool InitializeFromOBJ(const string& path);
	bool InitializeFromFBX(const string& path);

public:
	virtual bool Initialize(const string& path) override;
};