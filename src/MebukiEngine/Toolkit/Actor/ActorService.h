#pragma once
#include <ranges>

#include "Actor.h"
#include "Basic/UID.h"

class ActorService
{
public:
	ActorService() = default;
	~ActorService() = default;

	// Actorを生成するメソッド
	std::shared_ptr<Actor> Create(const std::wstring& name)
	{
		auto actor = std::make_shared<Actor>(name);
		actor->AddComponent<Transform>();
		actors.emplace(actor->GetUid(), actor);

		return actor;
	}

	// Actorを取得するメソッド
	template<typename T>
	std::shared_ptr<T> GetActor(const meb::uid& uid)
	{
		auto it = actors.find(uid);
		if (it != actors.end())
		{
			return std::static_pointer_cast<T>(it->second);
		}

		return nullptr;
	}

	std::ranges::elements_view
		<std::ranges::ref_view
		<const std::unordered_map
		<unsigned long long, std::shared_ptr<Actor>>>, 1> GetActors() const
	{
		return std::ranges::views::values(actors);
	}

	// Actorを破棄するメソッド
	void DestroyActor(const meb::uid& uid)
	{
		auto it = actors.find(uid);
		if (it != actors.end())
		{
			it->second->InvokeOnDestroy();
			actors.erase(it);
		}
	}

	void InvokeOnUpdate()
	{
		auto valuesView = std::ranges::views::values(actors);

		for (const auto& actor : valuesView)
		{
			actor->InvokeOnUpdate();
		}
	}

	void InvokeOnPreDraw(const GraphicsContext& context, GpuConstants& gpuConstants)
	{
		auto valuesView = std::ranges::views::values(actors);

		for (const auto& actor : valuesView)
		{
			actor->InvokeOnPreDraw(context, gpuConstants);
		}
	}


	void InvokeOnDraw(const GraphicsContext& context, const GpuConstants& gpuConstants)
	{
		auto valuesView = std::ranges::views::values(actors);

		for (const auto& actor : valuesView)
		{
			actor->InvokeOnDraw(context, gpuConstants);
		}
	}


private:
	std::unordered_map<meb::uid, std::shared_ptr<Actor>> actors;
};
