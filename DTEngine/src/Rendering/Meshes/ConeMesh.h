#pragma once

#include "Rendering/MeshBase.h"

class ConeMesh final : public MeshBase
{
public:
	ConeMesh();
	ConeMesh(const ConeMesh& other);
	virtual ~ConeMesh();

	virtual bool Initialize() override;
};