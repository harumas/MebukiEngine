#pragma once
#include "Mesh.h"

class PrimitiveMesh
{
public:
	static Mesh CreateSimpleTriangle();
	static Mesh CreateCube(float size = 1.0f);
};
