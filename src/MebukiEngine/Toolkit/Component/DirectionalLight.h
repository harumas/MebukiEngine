#pragma once
#include "Component.h"
#include "Toolkit/Math/Vec3.h"

class DirectionalLight : public Component
{
	struct LightingData
	{
		Vec3 lightDirection;
		Vec3 lightColor;
		Vec3 ambientLight;
	};

public:
	explicit DirectionalLight(const std::shared_ptr<Actor>& actorRef);

	LightingData currentData =
	{
		{ -0.3f, -0.3f, 0.8f },
		{ 0.8f, 0.8f, 0.8f },
		{ 0.4f, 0.4f, 0.4f }
	};

	void OnDraw(const GraphicsContext& context) override;

private:
	ConstantBuffer<LightingData> constantBuffer;
};
