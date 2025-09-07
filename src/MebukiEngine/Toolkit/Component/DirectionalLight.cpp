#include "DirectionalLight.h"

DirectionalLight::DirectionalLight(const std::shared_ptr<Actor>& actorRef) : Component(actorRef)
{
}

void DirectionalLight::OnDraw(const GraphicsContext& context)
{
	constantBuffer.SetBufferData(currentData);
	constantBuffer.SetConstantBufferView(context, 1);
}

