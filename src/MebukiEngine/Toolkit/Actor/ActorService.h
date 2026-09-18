#pragma once
#include <ranges>

#include "Actor.h"
#include "ActorRef.h"
#include "Basic/UID.h"
#include <Toolkit/Component/Transform.h>

struct ActorSlot
{
	uint32_t generation = 0;
	std::unique_ptr<Actor> actor = nullptr;
};

class ActorService
{
public:
	ActorService() = default;
	~ActorService() = default;

	ActorRef Create(const std::wstring& name);
	ActorRef GetRef(ActorHandle h);
	bool IsAlive(ActorHandle h) const;
	Actor& Get(ActorHandle h);

	void RequestDestroy(ActorHandle h);
	void ProcessDestroy();

	// 生存している全Actorを即座に破棄する（ワールド切り替え時などに使用）
	void Clear();

	void InvokeOnUpdate(float deltaTime);
	void InvokeOnPreDraw(const GraphicsContext& context, GpuConstants& gpuConstants);
	void InvokeOnDraw(RenderQueue& renderQueue);

private:
	std::vector<ActorSlot> slots;
	std::vector<uint32_t> freeList;
	std::queue<uint32_t> destroyRequestQueue;

	// 生きているActorだけにcallbackを適用する（ループ中の再確保・破棄予約を考慮したindexループ）
	void ForEachAliveActor(const std::function<void(Actor&)>& callback);
};
