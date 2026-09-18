#pragma once
#include <Toolkit/Component/Transform.h>
#include <Toolkit/Actor/ActorRef.h>

class Actor;
class ActorRef;

class Camera : public Component
{
public:
	explicit Camera(ActorRef actorRef);

	XMMATRIX GetViewMatrix() const;
	XMMATRIX GetProjectionMatrix(float aspectRatio) const;

	void Init(float fov, float nearPlane, float farPlane);
	void SetYaw(float yaw);
	void SetPitch(float pitch);

	void OnPreDraw(const GraphicsContext& context, GpuConstants& gpuConstants) override;

	std::shared_ptr<Transform> transform;

private:
	float fov;
	float nearPlane;
	float farPlane;
};