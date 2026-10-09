#pragma once
#include "Component.h"
#include <Rendering/GpuConstants.h>

class DirectionalLight : public Component
{
public:
	explicit DirectionalLight(ActorRef actorRef);

	DirectionalLightFrameData currentData =
	{
		{ -0.3f, -0.3f, 0.8f },
		{ 0.8f, 0.8f, 0.8f },
		{ 0.4f, 0.4f, 0.4f }
	};

	void OnPreDraw(const GraphicsContext& context, GpuConstants& gpuConstants) override;
};
