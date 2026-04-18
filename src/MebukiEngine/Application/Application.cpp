#include "Application.h"

#include "Basic/EngineService.h"
#include "Toolkit/Actor/ActorService.h"
#include "Toolkit/Rendering/MaterialHandler.h"
#include "Toolkit/Rendering/ShaderPassPool.h"


Application::Application(const WorldProfile& worldProfile) : worldManager(worldProfile)
{}

Application::~Application() = default;

void Application::Initialize(const WindowInfo& windowInfo)
{
	inputProvider.Initialize(windowInfo.hwnd);

	//描画パイプラインの初期化 
	renderPipeline.Initialize(windowInfo);

	// 描画処理のバインド
	renderPipeline.onRenderProcess.AddListener(std::bind(&Application::Render, this, std::placeholders::_1, std::placeholders::_2));

	// ServiceLocatorに各種機能を登録
	engineService.RegisterInstance<RenderPipeline>(std::shared_ptr<RenderPipeline>(&renderPipeline, [](RenderPipeline*) {}));
	engineService.Register<ShaderPassPool>(renderPipeline.GetRootSignature());
	engineService.Register<MaterialHandler>();
	actorService = engineService.Register<ActorService>();

	// 最初のワールドに切り替え
	worldManager.Switch(0, engineService);

	isInitialized = true;
}

int Application::Process(const WindowInfo& windowInfo)
{
	const auto& currentWorld = worldManager.GetCurrentWorld();

	// ワールドの更新
	currentWorld->Update();

	// フレームのレンダリング
	renderPipeline.RenderFrame(windowInfo);

	// 入力バッファの更新
	inputProvider.SwapBuffer();

	return 1;
}

void Application::ProcessInput(const LPARAM& lparam)
{
	// 入力の処理
	inputProvider.Process(lparam);
}

bool Application::ProcessWin32Message(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (!isInitialized)
		return false;

	// フォーカスを失った際にキー入力状態をリセット
	// （非アクティブ中のキーリリースはWM_INPUTが届かないため取りこぼされる）
	if (msg == WM_KILLFOCUS)
	{
		inputProvider.ClearAllKeys();
		return false;
	}

	// ImGui に Win32 メッセージを転送する
	return renderPipeline.GetImGuiRenderer().ProcessWin32Message(hwnd, msg, wParam, lParam);
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

	// 描画前の更新処理(カメラ, ライト etc...) 
	actorService->InvokeOnPreDraw(context, gpuConstants);

	// TransformCBの定数バッファを転送
	gpuConstants.UploadTransformBuffer();

	// FrameCBの定数バッファの転送とセット
	gpuConstants.UploadFrameBuffer();
	gpuConstants.SetFrameCBV(context);

	// Componentの描画処理
	actorService->InvokeOnDraw(context, gpuConstants);

	gpuConstants.Reset();
}
