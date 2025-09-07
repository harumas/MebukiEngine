#pragma once
#include "Rendering/RenderPipeline.h"
#include "Toolkit/World/WorldManager.h"
#include "Toolkit/World/WorldProfile.h"

class ActorService;
class EngineService;

class Application
{
public:
	Application(const WorldProfile& worldProfile);
	~Application();

	void Initialize(const WindowInfo& windowInfo);
	int Process(const WindowInfo& windowInfo);
	void Finalize();

private:
	RenderPipeline renderPipeline;
	WorldManager worldManager;
	std::shared_ptr<ActorService> actorService;

	void Render(const GraphicsContext& context);
};

