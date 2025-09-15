#pragma once
#include <memory>

#include <Toolkit/Entity/Entity.h>
#include <Toolkit/Component/Component.h>
#include <Toolkit/Component/Transform.h>

class Actor : public Entity, public std::enable_shared_from_this<Actor>
{
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

		std::shared_ptr<T> component = std::make_shared<T>(shared_from_this());
		components.emplace(std::type_index(typeid(T)), component);
		component->OnCreate();

		return component;
	}

	template<typename T>
	std::shared_ptr<T> GetComponent()
	{
		return std::dynamic_pointer_cast<T>(components[std::type_index(typeid(T))]);
	}

	void InvokeOnUpdate();
	void InvokeOnPreDraw(const GraphicsContext& context, GpuConstants& gpuConstants);
	void InvokeOnDraw(const GraphicsContext& context, const GpuConstants& gpuConstants);
	void InvokeOnDestroy();

private:
	std::unordered_map<std::type_index, std::shared_ptr<Component>> components;
	std::weak_ptr<Actor> self;
};

