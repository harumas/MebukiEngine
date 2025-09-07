#pragma once
#include <Toolkit/Component/Transform.h>

class Actor;

class Camera : public Component
{
public:
	explicit Camera(const std::shared_ptr<Actor>& actorRef);

	XMMATRIX GetViewMatrix() const;
	XMMATRIX GetProjectionMatrix(float aspectRatio) const;

	void Init(float fov, float nearPlane, float farPlane);
	void SetYaw(float yaw);
	void SetPitch(float pitch);

	std::shared_ptr<Transform> transform;
	static std::shared_ptr<Camera> current;

private:
	float fov;
	float nearPlane;
	float farPlane;
};