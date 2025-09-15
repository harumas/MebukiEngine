#pragma once
#include "Component.h"

class DirectionalLight : public Component
{
public:
	explicit DirectionalLight(const std::shared_ptr<Actor>& actorRef);

	DirectionalLightFrameData currentData =
	{
		{ -0.3f, -0.3f, 0.8f },
		{ 0.8f, 0.8f, 0.8f },
		{ 0.4f, 0.4f, 0.4f }
	};

	void OnPreDraw(const GraphicsContext& context, GpuConstants& gpuConstants) override;
};
