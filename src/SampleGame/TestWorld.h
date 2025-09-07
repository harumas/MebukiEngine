#pragma once
#include <Toolkit/World.h>
#include <Toolkit/ActorComponent.h>
#include <Toolkit/Rendering.h>

class TestWorld final :public World
{
public:
	TestWorld() = default;
	~TestWorld() override = default;

	void Initialize() override;
	void Update() override;

	std::string_view GetName() override
	{
		return "TestWorld";
	}

private:
	std::shared_ptr<Actor> cube;
};
