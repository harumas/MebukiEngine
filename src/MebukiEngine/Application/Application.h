#pragma once
#include "Rendering/GpuConstants.h"
#include "Rendering/RenderPipeline.h"
#include "Toolkit/World/WorldManager.h"
#include "Toolkit/World/WorldProfile.h"
#include <Input/InputProvider.h>

class ActorService;
class EngineService;

class Application
{
public:
	Application(const WorldProfile& worldProfile);
	~Application();

	void Initialize(const WindowInfo& windowInfo);
	int Process(const WindowInfo& windowInfo);
	void ProcessInput(const LPARAM& lparam);
	bool ProcessWin32Message(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	void Finalize();

private:
	bool isInitialized = false;
	RenderPipeline renderPipeline;
	WorldManager worldManager;
	EngineService engineService;
	InputProvider inputProvider;
	std::shared_ptr<ActorService> actorService;

	void Render(const GraphicsContext& context, GpuConstants& gpuConstants);
};

