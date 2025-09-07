#pragma once
#include "Component.h"
#include <Toolkit/Mathematics.h>


class PointLight : public Component
{
	struct LightingData
	{
		DirectX::XMVECTOR lightPosition;
		DirectX::XMVECTOR lightColor;
		float radius;
	};

public:
	explicit PointLight(const std::shared_ptr<Actor>& actorRef);

	LightingData currentData =
	{
		{ 0.0f, 0.0f, 0.0f },
		{ 0.8f, 0.8f, 1.0f },
		3.0f
	};

	void OnDraw(const GraphicsContext& context) override;

private:
	ConstantBuffer<LightingData> constantBuffer;
};

