#include "Transform.h"

Transform::Transform(const std::shared_ptr<Actor>& actorRef) :
	Component(actorRef),
	position(0, 0, 0),
	rotation(0, 0, 0),
	scale(1, 1, 1)
{
}

Vec3 Transform::Forward() const
{
	const XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYawFromVector(rotation);
	return Vec3(XMVector3TransformNormal(Vec3::FORWARD, rotationMatrix));
}

Vec3 Transform::Right() const
{
	const XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYawFromVector(rotation);
	return Vec3(XMVector3TransformNormal(Vec3::RIGHT, rotationMatrix));
}

Vec3 Transform::Up() const
{
	const XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYawFromVector(rotation);
	return Vec3(XMVector3TransformNormal(Vec3::UP, rotationMatrix));
}

XMMATRIX Transform::GetMatrix() const
{
	return XMMatrixScalingFromVector(scale)
		* XMMatrixRotationRollPitchYawFromVector(rotation)
		* XMMatrixTranslationFromVector(position);
}
