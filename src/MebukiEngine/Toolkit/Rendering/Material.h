#pragma once
#include <Rendering/Rendering.h>	
#include <Toolkit/Entity/Entity.h>

class Material : public Entity
{
public:
	explicit Material(const std::shared_ptr<ShaderPass>& shaderPass);
	void SetPass(const GraphicsContext& context) const;

private:
	std::shared_ptr<ShaderPass> shaderPass;
};

