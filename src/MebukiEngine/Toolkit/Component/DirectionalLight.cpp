#include "DirectionalLight.h"

DirectionalLight::DirectionalLight(const std::shared_ptr<Actor>& actorRef) : Component(actorRef)
{
}

void DirectionalLight::OnPreDraw(const GraphicsContext& context, GpuConstants& gpuConstants)
{
	gpuConstants.SetDirectionalLightFrameData(currentData);
}

