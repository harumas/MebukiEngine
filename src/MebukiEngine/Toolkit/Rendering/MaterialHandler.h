#pragma once
#include "Toolkit/Rendering/Material.h"

class MaterialHandler
{
public:
	Material Create(const std::shared_ptr<ShaderPass>& shaderPass)
	{
		uint32_t handleId = handleCount++;
		Material mat(handleId, shaderPass);
		return mat;
	}

private:
	UINT handleCount = 0;
};
