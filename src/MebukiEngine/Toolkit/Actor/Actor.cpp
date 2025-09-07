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

void Actor::InvokeOnDraw(const GraphicsContext& context)
{
	auto valuesView = std::ranges::views::values(components);

	for (const auto& component : valuesView)
	{
		component->OnDraw(context);
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
