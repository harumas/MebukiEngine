#include "Application.h"

#include "Basic/EngineService.h"
#include "Toolkit/Actor/ActorService.h"
#include "Toolkit/Rendering/MaterialHandler.h"
#include "Toolkit/Rendering/ShaderPassPool.h"


Application::Application(const WorldProfile& worldProfile) : worldManager(worldProfile)
{
}

Application::~Application() = default;

void Application::Initialize(const WindowInfo& windowInfo)
{
	//描画パイプラインの初期化 
	renderPipeline.Initialize(windowInfo);

	// 描画処理のバインド
	renderPipeline.onRenderProcess.AddListener(std::bind(&Application::Render, this, std::placeholders::_1, std::placeholders::_2));

	// ServiceLocatorに各種機能を登録
	engineService.Register<ShaderPassPool>(renderPipeline.GetRootSignature());
	engineService.Register<MaterialHandler>();
	actorService = engineService.Register<ActorService>();

	// 最初のワールドに切り替え
	worldManager.Switch(0, engineService);
}

int Application::Process(const WindowInfo& windowInfo)
{
	const auto& currentWorld = worldManager.GetCurrentWorld();

	// ワールドの更新
	currentWorld->Update();


	// フレームのレンダリング
	renderPipeline.RenderFrame(windowInfo);

	return 1;
}

void Application::Finalize()
{
	// 描画パイプラインの破棄処理 
	renderPipeline.Finalize();
}

void Application::Render(const GraphicsContext& context, GpuConstants& gpuConstants)
{
	// Componentの更新
	actorService->InvokeOnUpdate();

	// TransformCBの定数バッファを転送
	gpuConstants.UploadTransformBuffer();

	// 描画前の更新処理(カメラ, ライト etc...) 
	actorService->InvokeOnPreDraw(context, gpuConstants);

	// FrameCBの定数バッファの転送とセット
	gpuConstants.UploadFrameBuffer();
	gpuConstants.SetFrameCBV(context);

	// Componentの描画処理
	actorService->InvokeOnDraw(context, gpuConstants);

	gpuConstants.Reset();
}
