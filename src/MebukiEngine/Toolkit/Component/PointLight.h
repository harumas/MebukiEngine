#pragma once
#include "Component.h"

class PointLight : public Component
{
public:
	explicit PointLight(const std::shared_ptr<Actor>& actorRef);

	void OnPreDraw(const GraphicsContext& context, GpuConstants& gpuConstants) override;

	PointLightFrameData currentData =
	{
		{ 0.0f, 0.0f, 0.0f },
		{ 0.8f, 0.8f, 1.0f },
		3.0f
	};
};

