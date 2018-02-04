#pragma once

#include "Rendering/MeshBase.h"

class HexagonMesh final : public MeshBase
{
public:
	HexagonMesh();
	HexagonMesh(const HexagonMesh& other);
	virtual ~HexagonMesh();

	virtual bool Initialize() override;
};