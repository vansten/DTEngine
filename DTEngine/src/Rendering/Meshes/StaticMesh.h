#pragma once

#include "Rendering/MeshBase.h"

class StaticMesh : public MeshBase
{
private:
	std::vector<VertexType> _vertices;
	std::vector<unsigned int> _indices;

public:
	StaticMesh();
	StaticMesh(const StaticMesh& other);
	virtual ~StaticMesh();

protected:
	bool LoadFromOBJ(const String& path);
	bool LoadFromFBX(const String& path);

public:
	virtual bool Load(const String& path) override;

	virtual bool Initialize() override;
};