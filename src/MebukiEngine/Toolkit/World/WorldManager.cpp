#include "WorldManager.h"

void WorldManager::Switch(int index)
{
	if (currentScene != nullptr)
	{
		currentScene.reset();
	}

	if (index < factory->size())
	{
		currentScene = factory->at(index)();
		currentScene->Initialize();
		return;
	}

	ThrowMessage("Scene " + std::to_string(index) + " does not exist!");
}

const std::unique_ptr<World>& WorldManager::GetCurrentScene()
{
	return currentScene;
}


