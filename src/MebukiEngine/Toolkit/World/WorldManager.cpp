#include "WorldManager.h"

void WorldManager::Switch(int index, const EngineService& engineService)
{
	if (currentWorld != nullptr)
	{
		currentWorld.reset();
	}

	if (index < factory->size())
	{
		currentWorld = factory->at(index)();
		currentWorld->Initialize(engineService);
		return;
	}

	ThrowMessage("World " + std::to_string(index) + " does not exist!");
}

const std::unique_ptr<World>& WorldManager::GetCurrentWorld()
{
	return currentWorld;
}

