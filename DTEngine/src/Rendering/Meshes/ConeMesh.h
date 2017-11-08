#pragma once

#include "Rendering/MeshBase.h"

class ConeMesh : public MeshBase
{
public:
	ConeMesh();
	ConeMesh(const ConeMesh& other);
	virtual ~ConeMesh();

	virtual bool Initialize(const String& path) override;
};