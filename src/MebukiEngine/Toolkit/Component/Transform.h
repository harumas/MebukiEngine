#pragma once
#include <Toolkit/Mathematics.h>
#include <Toolkit/Component/Component.h>

class Transform : public Component
{
public:
	Vec3 position;
	Vec3 rotation;
	Vec3 scale;

	explicit Transform(ActorRef actor);

	Vec3 Forward() const;
	Vec3 Right() const;
	Vec3 Up() const;

	XMMATRIX GetMatrix() const;
};

