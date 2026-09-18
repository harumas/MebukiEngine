#include "ActorService.h"

ActorRef ActorService::Create(const std::wstring& name)
{
	uint32_t index;

	if (!freeList.empty())
	{
		index = freeList.back();
		freeList.pop_back();
	}
	else
	{
		index = static_cast<uint32_t>(slots.size());
		slots.emplace_back();
	}

	auto& slot = slots[index];
	slot.generation++;
	slot.actor = std::make_unique<Actor>(name);

	ActorHandle handle{ index, slot.generation };
	slot.actor->selfRef = ActorRef(handle, this);
	slot.actor->AddComponent<Transform>();

	return ActorRef(handle, this);
}

ActorRef ActorService::GetRef(ActorHandle h)
{
	return ActorRef(h, this);
}

bool ActorService::IsAlive(ActorHandle h) const
{
	if (h.index >= slots.size())
	{
		return false;
	}

	return slots[h.index].generation == h.generation;
}

Actor& ActorService::Get(ActorHandle h)
{
	assert(IsAlive(h));
	return *slots[h.index].actor;
}

void ActorService::RequestDestroy(ActorHandle h)
{
	if (h.index >= slots.size())
	{
		return;
	}

	// すでに世代が異なる場合は終了
	auto& slot = slots[h.index];
	if (slot.generation != h.generation)
	{
		return;
	}

	destroyRequestQueue.push(h.index);
	slot.generation++;
}

void ActorService::ProcessDestroy()
{
	while (destroyRequestQueue.size() > 0)
	{
		uint32_t index = destroyRequestQueue.front();
		destroyRequestQueue.pop();

		// Actorを破棄する
		auto& slot = slots[index];
		slot.actor->InvokeOnDestroy();
		slots[index].actor.reset();

		// 解放済みリストに登録する
		freeList.push_back(index);
	}
}

void ActorService::Clear()
{
	for (auto& slot : slots)
	{
		if (slot.actor)
		{
			slot.actor->InvokeOnDestroy();
			slot.actor.reset();
		}
	}

	slots.clear();
	freeList.clear();

	while (!destroyRequestQueue.empty())
	{
		destroyRequestQueue.pop();
	}
}

void ActorService::InvokeOnUpdate(float deltaTime)
{
	ForEachAliveActor([deltaTime](Actor& actor)
	{
		actor.InvokeOnUpdate(deltaTime);
	});
}

void ActorService::InvokeOnPreDraw(const GraphicsContext& context, GpuConstants& gpuConstants)
{
	ForEachAliveActor([&context, &gpuConstants](Actor& actor)
	{
		actor.InvokeOnPreDraw(context, gpuConstants);
	});
}

void ActorService::InvokeOnDraw(RenderQueue& renderQueue)
{
	ForEachAliveActor([&renderQueue](Actor& actor)
	{
		actor.InvokeOnDraw(renderQueue);
	});
}

void ActorService::ForEachAliveActor(const std::function<void(Actor&)>& callback)
{
	size_t actorCount = slots.size();

	for (size_t i = 0; i < actorCount; i++)
	{
		auto& slot = slots[i];

		if (slot.actor != nullptr && IsAlive(slot.actor->selfRef.GetHandle()))
		{
			callback(*slot.actor);
		}
	}
}
