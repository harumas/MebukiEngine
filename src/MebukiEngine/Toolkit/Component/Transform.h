#pragma once
#include <Toolkit/Mathematics.h>
#include <Toolkit/Actor/Actor.h>
#include <Toolkit/Component/Component.h>

class Transform : public Component
{
public:
	Vec3 position;
	Vec3 rotation;
	Vec3 scale;

	explicit Transform(const std::shared_ptr<Actor>& actorRef);

	Vec3 Forward() const;
	Vec3 Right() const;
	Vec3 Up() const;

	XMMATRIX GetMatrix() const;
};

