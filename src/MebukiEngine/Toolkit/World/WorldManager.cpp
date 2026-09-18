#include "WorldManager.h"
#include "Basic/EngineService.h"
#include "Toolkit/Actor/ActorService.h"

void WorldManager::Switch(int index, const EngineService& engineService)
{
	if (currentWorld != nullptr)
	{
		currentWorld.reset();
	}

	// 前のワールドが生成したActorを全て破棄する
	engineService.Resolve<ActorService>()->Clear();

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

