#include "Application.h"

#include "Basic/EngineService.h"
#include "Toolkit/Actor/ActorService.h"
#include "Toolkit/Rendering/ShaderPassPool.h"


Application::Application(const WorldProfile& worldProfile) : worldManager(worldProfile)
{
}

Application::~Application() = default;

void Application::Initialize(const WindowInfo& windowInfo)
{
	renderPipeline.Initialize(windowInfo);
	EngineService::GetInstance().Register<ShaderPassPool>(renderPipeline.GetRootSignature());
	actorService = EngineService::GetInstance().Register<ActorService>();

	worldManager.Switch(0);

	renderPipeline.onRenderProcess.AddListener(std::bind(&Application::Render, this, std::placeholders::_1));
}

int Application::Process(const WindowInfo& windowInfo)
{
	const auto& currentScene = worldManager.GetCurrentScene();
	currentScene->Update();
	actorService->InvokeOnUpdate();
	renderPipeline.Update(windowInfo);
	return 1;
}

void Application::Finalize()
{
	renderPipeline.Finalize();
}

void Application::Render(const GraphicsContext& context)
{
	actorService->InvokeOnDraw(context);
}
