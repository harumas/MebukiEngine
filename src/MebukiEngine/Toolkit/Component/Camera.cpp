#include "Camera.h"

std::shared_ptr<Camera> Camera::current = nullptr;

Camera::Camera(const std::shared_ptr<Actor>& actorRef) :
	Component(actorRef),
	fov(XM_PIDIV2* (2.0 / 3.0)), //60 degrees
	nearPlane(0.1f),
	farPlane(1000.0f)
{
	transform = actorRef->GetComponent<Transform>();
}

void Camera::Init(float fov, float nearPlane, float farPlane)
{
	this->fov = fov;
	this->nearPlane = nearPlane;
	this->farPlane = farPlane;
}

XMMATRIX Camera::GetViewMatrix() const
{
	const XMVECTOR& rotation = XMQuaternionRotationRollPitchYawFromVector(transform->rotation);
	const XMMATRIX rotationMatrix = XMMatrixRotationQuaternion(rotation);
	const XMVECTOR eyeDirection = XMVector3TransformNormal(Vec3::FORWARD, rotationMatrix);

	return XMMatrixLookToLH(transform->position, eyeDirection, Vec3::UP);
}

XMMATRIX Camera::GetProjectionMatrix(float aspectRatio) const
{
	return XMMatrixPerspectiveFovLH(fov, aspectRatio, nearPlane, farPlane);
}

void Camera::SetYaw(float yaw)
{
	transform->rotation.y = yaw;
}

void Camera::SetPitch(float pitch)
{
	transform->rotation.x = pitch;
}
