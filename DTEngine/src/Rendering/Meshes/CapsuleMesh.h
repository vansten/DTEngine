#pragma once

#include "Rendering/MeshBase.h"

class CapsuleMesh : public MeshBase
{
public:
	CapsuleMesh();
	CapsuleMesh(const CapsuleMesh& other);
	virtual ~CapsuleMesh();

	virtual bool Initialize() override;
};