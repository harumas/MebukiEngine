#pragma once
#include <memory>

#include <Toolkit/Entity/Entity.h>
#include <Toolkit/Component/Component.h>
#include <Toolkit/Actor/ActorRef.h>


class Actor : public Entity
{
	friend class ActorService;

public:
	std::wstring name;

	explicit Actor(const std::wstring& name);

	template<typename T>
	std::shared_ptr<T> AddComponent()
	{
		if (components.contains(typeid(T)))
		{
			ThrowMessage("同じ型のコンポーネントは追加できません");
			return nullptr;
		}

		std::shared_ptr<T> component = std::make_shared<T>(selfRef);
		components.emplace(std::type_index(typeid(T)), component);
		component->OnCreate();

		return component;
	}

	/// @brief Actorから指定されたコンポーネントを取得する
	/// @tparam T Componentの型
	/// @return Componentのshared_ptr
	template<typename T>
	std::shared_ptr<T> GetComponent()
	{
		const auto it = components.find(std::type_index(typeid(T)));

		// コンポーネントが存在しない場合はnullptrを返す 
		if (it == components.end())
		{
			return nullptr;
		}

		return std::dynamic_pointer_cast<T>(it->second);
	}

	void InvokeOnUpdate(float deltaTime);
	void InvokeOnPreDraw(const GraphicsContext& context, GpuConstants& gpuConstants);
	void InvokeOnDraw(RenderQueue& renderQueue);
	void InvokeOnDestroy();

private:
	std::unordered_map<std::type_index, std::shared_ptr<Component>> components;
	ActorRef selfRef;
};

