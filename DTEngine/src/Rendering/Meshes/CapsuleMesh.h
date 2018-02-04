#pragma once

#include "Rendering/MeshBase.h"

class CapsuleMesh final : public MeshBase
{
public:
	CapsuleMesh();
	CapsuleMesh(const CapsuleMesh& other);
	virtual ~CapsuleMesh();

	virtual bool Initialize() override;
};