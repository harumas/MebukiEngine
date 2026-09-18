#pragma once
#include "Mesh.h"

class PrimitiveMesh
{
public:
	static Mesh CreateSimpleTriangle(ID3D12Device* device);
	static Mesh CreateCube(ID3D12Device* device, float size = 1.0f);
};
