#pragma once
#include "Actor.h"
#include <ranges>
#include <Toolkit/Component/Component.h>

Actor::Actor(const std::wstring& name) :
	name(name)
{
}

void Actor::InvokeOnUpdate(float deltaTime)
{
	auto valuesView = std::ranges::views::values(components);

	for (const auto& component : valuesView)
	{
		component->OnUpdate(deltaTime);
	}
}

void Actor::InvokeOnPreDraw(const GraphicsContext& context, GpuConstants& gpuConstants)
{
	auto valuesView = std::ranges::views::values(components);

	for (const auto& component : valuesView)
	{
		component->OnPreDraw(context, gpuConstants);
	}
}

void Actor::InvokeOnDraw(RenderQueue& renderQueue)
{
	auto valuesView = std::ranges::views::values(components);

	for (const auto& component : valuesView)
	{
		component->OnDraw(renderQueue);
	}
}

void Actor::InvokeOnDestroy()
{
	auto valuesView = std::ranges::views::values(components);

	for (const auto& component : valuesView)
	{
		component->OnDestroy();
	}
}
