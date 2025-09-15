#pragma once
#include "Actor.h"
#include <ranges>
#include <Toolkit/Component/Component.h>

Actor::Actor(const std::wstring& name) :
	name(name)
{
}

void Actor::InvokeOnUpdate()
{
	auto valuesView = std::ranges::views::values(components);

	for (const auto& component : valuesView)
	{
		component->OnUpdate();
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

void Actor::InvokeOnDraw(const GraphicsContext& context, const GpuConstants& gpuConstants)
{
	auto valuesView = std::ranges::views::values(components);

	for (const auto& component : valuesView)
	{
		component->OnDraw(context, gpuConstants);
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
