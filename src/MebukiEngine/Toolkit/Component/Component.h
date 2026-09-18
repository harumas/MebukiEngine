#pragma once
#include <Toolkit/Entity/Entity.h>
#include <Toolkit/Actor/ActorRef.h>

class Actor;
class ActorRef;
class GraphicsContext;
class GpuConstants;
class RenderQueue;

class Component : public Entity
{
public:
	ActorRef actor;

	explicit Component(ActorRef actorRef);

	virtual ~Component() = default;
	virtual void OnCreate() {}
	virtual void OnUpdate(float deltaTime) {}
	virtual void OnPreDraw(const GraphicsContext& context, GpuConstants& gpuConstants) {}
	virtual void OnDraw(RenderQueue& renderQueue) {}
	virtual void OnDestroy() {}

private:
	Component(const Component&) = delete;
	Component& operator=(const Component&) = delete;

	Component(const Component&&) = delete;
	Component& operator=(const Component&&) = delete;
};
