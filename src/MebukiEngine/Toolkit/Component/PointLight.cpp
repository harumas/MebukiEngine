#include "PointLight.h"

PointLight::PointLight(const std::shared_ptr<Actor>& actorRef) : Component(actorRef)
{
}

void PointLight::OnPreDraw(const GraphicsContext& context, GpuConstants& gpuConstants)
{
	gpuConstants.SetPointLightFrameData(currentData);
}

