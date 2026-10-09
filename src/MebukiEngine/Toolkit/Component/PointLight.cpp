#include "PointLight.h"
#include <Rendering/GraphicsContext.h>
#include <Rendering/GpuConstants.h>

PointLight::PointLight(ActorRef actorRef) : Component(actorRef)
{
}

void PointLight::OnPreDraw(const GraphicsContext& context, GpuConstants& gpuConstants)
{
	gpuConstants.SetPointLightFrameData(currentData);
}

