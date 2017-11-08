#pragma once

#include "Rendering/MeshBase.h"

class HexagonMesh : public MeshBase
{
public:
	HexagonMesh();
	HexagonMesh(const HexagonMesh& other);
	virtual ~HexagonMesh();

	virtual bool Initialize(const String& path) override;
};