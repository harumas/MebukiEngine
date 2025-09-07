#include "PointLight.h"

PointLight::PointLight(const std::shared_ptr<Actor>& actorRef) : Component(actorRef)
{
}

void PointLight::OnDraw(const GraphicsContext& context)
{
	constantBuffer.SetBufferData(currentData);
	constantBuffer.SetConstantBufferView(context, 2);
}
