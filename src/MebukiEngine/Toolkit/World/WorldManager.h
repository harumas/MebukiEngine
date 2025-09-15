#pragma once
#include <functional>
#include <memory>
#include "World.h"
#include "WorldProfile.h"

class WorldManager
{
	using FactoryFunction = std::function<std::unique_ptr<World>()>;

public:
	explicit WorldManager(const WorldProfile& worldProfile) :
		factory(worldProfile.GetFactory())
	{
	}

	void Switch(int index, const EngineService& engineService);

	const std::unique_ptr<World>& GetCurrentWorld();

private:
	std::unique_ptr<World> currentWorld;
	std::shared_ptr<std::vector<FactoryFunction>> factory;
};
