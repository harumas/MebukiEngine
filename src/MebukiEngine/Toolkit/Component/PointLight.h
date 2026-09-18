#pragma once
#include "Component.h"
#include <Rendering/GpuConstants.h>

class PointLight : public Component
{
public:
	explicit PointLight(ActorRef actorRef);

	void OnPreDraw(const GraphicsContext& context, GpuConstants& gpuConstants) override;

	PointLightFrameData currentData =
	{
		{ 0.0f, 0.0f, 0.0f },
		{ 0.8f, 0.8f, 1.0f },
		3.0f
	};
};

