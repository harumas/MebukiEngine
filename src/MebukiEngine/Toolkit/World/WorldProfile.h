#pragma once
#include "World.h"

class WorldProfile
{
	using FactoryFunction = std::function<std::unique_ptr<World>()>;

public:
	WorldProfile() : factory(std::make_shared<std::vector<FactoryFunction>>())
	{
	}

	~WorldProfile() = default;

	std::shared_ptr<std::vector<FactoryFunction>> GetFactory() const
	{
		return factory;
	}

	template <typename T>
	void Register()
	{
		factory->emplace_back([]() { return std::make_unique<T>(); });
	}


private:
	std::shared_ptr<std::vector<FactoryFunction>> factory;
};

