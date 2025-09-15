#pragma once
#include <Rendering/Rendering.h>
#include <Toolkit/Entity/Entity.h>

#include "Rendering/GpuConstants.h"

class Actor;

class Component : public Entity
{
public:
	std::weak_ptr<Actor> gameObject;

	explicit Component(const std::shared_ptr<Actor>& actorRef);

	virtual ~Component() = default;
	virtual void OnCreate() {}
	virtual void OnUpdate() {}
	virtual void OnPreDraw(const GraphicsContext& context, GpuConstants& gpuConstants) {}
	virtual void OnDraw(const GraphicsContext& context, const GpuConstants& gpuConstants) {}
	virtual void OnDestroy() {}

private:
	Component(const Component&) = delete;
	Component& operator=(const Component&) = delete;

	Component(const Component&&) = delete;
	Component& operator=(const Component&&) = delete;
};
