#include "Material.h"


Material::Material(const std::shared_ptr<ShaderPass>& shaderPass) : shaderPass(shaderPass)
{
}

void Material::SetPass(const GraphicsContext& context) const
{
	if (shaderPass != nullptr)
	{
		auto pipelineState = shaderPass->GetPipelineState();
		context.SetPipelineState(pipelineState.get());
	}
	else
	{
		ThrowMessage("ShaderPass is not set for Material.");
	}
}
