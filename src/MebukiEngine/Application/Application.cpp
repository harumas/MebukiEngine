#include "Application.h"
#include "Basic/Profiler.h"
#include <psapi.h>

#include "Basic/EngineService.h"
#include "Rendering/GraphicsDevice.h"
#include "Toolkit/Actor/ActorService.h"
#include "Toolkit/Rendering/MaterialHandler.h"
#include "Toolkit/Rendering/ShaderPassPool.h"


Application::Application(const WorldProfile& worldProfile) : worldManager(worldProfile)
{}

Application::~Application() = default;

void Application::Initialize(const WindowInfo& windowInfo)
{
	ScopedTimer initializeTimer("Initialize (total)");

	inputProvider.Initialize(windowInfo.hwnd);

	//描画パイプラインの初期化
	{
		ScopedTimer timer("  RenderPipeline::Initialize");
		renderPipeline.Initialize(windowInfo);
	}

	// シャドウマップ・シャドウ用シェーダーパスの作成 (RenderPipelineはシャドウの存在を知らない)
	{
		ScopedTimer timer("  ShadowMap + ShadowPass compile");
		shadowMap = std::make_unique<ShadowMap>(renderPipeline.GetDevice().get(), renderPipeline.GetGpuConstants(), 2048, 2048);
		shadowShaderPass = std::make_shared<ShadowPass>();
		shadowShaderPass->Compile(renderPipeline.GetDevice().get(), renderPipeline.GetRootSignature());
	}

	// 描画処理のバインド
	renderPipeline.onRenderProcess.AddListener(std::bind(&Application::Render, this, std::placeholders::_1, std::placeholders::_2));

	// ServiceLocatorに各種機能を登録
	engineService.RegisterInstance<RenderPipeline>(std::shared_ptr<RenderPipeline>(&renderPipeline, [](RenderPipeline*) {}));
	engineService.RegisterInstance<GraphicsDevice>(std::make_shared<GraphicsDevice>(renderPipeline.GetDevice()));
	engineService.Register<ShaderPassPool>(renderPipeline.GetDevice().get(), renderPipeline.GetRootSignature());
	engineService.Register<MaterialHandler>();
	actorService = engineService.Register<ActorService>();

	// 最初のワールドに切り替え
	{
		ScopedTimer timer("  World::Initialize (total)");
		worldManager.Switch(0, engineService);
	}

	isInitialized = true;
}

int Application::Process(const WindowInfo& windowInfo)
{
	// Render() 内でメインの描画対象に戻すために必要
	currentWindowInfo = windowInfo;

	const auto& currentWorld = worldManager.GetCurrentWorld();

	// 経過時間を更新
	time.Tick();
	const float deltaTime = time.GetDeltaTime();

	// ワールドの更新
	{
		ScopedTimer timer("Frame: Update (World + Components)");
		currentWorld->Update(deltaTime);

		// Componentの更新
		actorService->InvokeOnUpdate(deltaTime);
	}

	// 破棄予約されたActorを実際に破棄する
	actorService->ProcessDestroy();

	// フレームのレンダリング
	{
		ScopedTimer timer("Frame: RenderFrame (total)");
		renderPipeline.RenderFrame(windowInfo);
	}

	// 入力バッファの更新
	inputProvider.SwapBuffer();

	ReportProfile();

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

void Application::Resize(UINT width, UINT height)
{
	// Initialize前にWM_SIZEが届くことがあるため、初期化済みか確認する
	if (!isInitialized)
		return;

	renderPipeline.Resize(width, height);
}

void Application::Finalize()
{
	// 描画パイプラインの破棄処理 
	renderPipeline.Finalize();
}

void Application::Render(const GraphicsContext& context, GpuConstants& gpuConstants)
{
	// 描画前の更新処理(カメラ, ライト etc...) ここでDirectionalLightがlightViewProjも計算する
	{
		ScopedTimer timer("  Frame: OnPreDraw");
		actorService->InvokeOnPreDraw(context, gpuConstants);
	}

	// TransformCBの定数バッファを転送
	gpuConstants.UploadTransformBuffer();

	// FrameCBの定数バッファの転送とセット (lightViewProjを含む。シャドウパスがこれを使うので先に行う)
	gpuConstants.UploadFrameBuffer();
	gpuConstants.SetFrameCBV(context);

	// Componentの描画データを収集する (この時点ではGPUコマンドはまだ発行しない)
	{
		ScopedTimer timer("  Frame: OnDraw (collect)");
		actorService->InvokeOnDraw(renderQueue);
	}

	ID3D12GraphicsCommandList* commandList = context.GetCommandList();

	// --- シャドウパス: シャドウマップに深度だけ描く ---
	D3D12_CPU_DESCRIPTOR_HANDLE shadowDsvHandle = shadowMap->GetDSVHandle();
	commandList->OMSetRenderTargets(0, nullptr, false, &shadowDsvHandle);
	commandList->ClearDepthStencilView(shadowDsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	CD3DX12_VIEWPORT shadowViewport(0.0f, 0.0f, static_cast<float>(shadowMap->GetWidth()), static_cast<float>(shadowMap->GetHeight()));
	CD3DX12_RECT shadowScissor(0, 0, static_cast<LONG>(shadowMap->GetWidth()), static_cast<LONG>(shadowMap->GetHeight()));
	commandList->RSSetViewports(1, &shadowViewport);
	commandList->RSSetScissorRects(1, &shadowScissor);

	context.SetPipelineState(shadowShaderPass->GetPipelineState().get());
	{
		ScopedTimer timer("  Frame: shadow pass (record)");
		renderQueue.FlushShadow(context, gpuConstants);
	}

	// シャドウマップをメインパスでSRVとして読めるように状態遷移
	D3D12_RESOURCE_BARRIER shadowToSrv = CD3DX12_RESOURCE_BARRIER::Transition(
		shadowMap->Get(), D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	context.ResourceBarrier(1, &shadowToSrv);

	// --- メインパスの描画対象・ビューポートに戻す (シャドウパスで上書きされているため) ---
	D3D12_CPU_DESCRIPTOR_HANDLE mainRtvHandle = renderPipeline.GetRenderTargetBuffer()->GetRTVHandle();
	D3D12_CPU_DESCRIPTOR_HANDLE mainDsvHandle = renderPipeline.GetDepthStencilBuffer()->GetDSVHandle();
	commandList->OMSetRenderTargets(1, &mainRtvHandle, true, &mainDsvHandle);

	CD3DX12_VIEWPORT mainViewport(0.0f, 0.0f, static_cast<float>(currentWindowInfo.width), static_cast<float>(currentWindowInfo.height));
	CD3DX12_RECT mainScissor(0, 0, currentWindowInfo.width, currentWindowInfo.height);
	commandList->RSSetViewports(1, &mainViewport);
	commandList->RSSetScissorRects(1, &mainScissor);

	// シャドウマップのSRVをバインド (FrameCBは既に上でセット済み)
	gpuConstants.SetShadowMapSRV(context, shadowMap->GetSRVHandle());

	// --- メインパス: 集めておいた描画データを実際のマテリアルで発行する ---
	{
		ScopedTimer timer("  Frame: main pass (record)");
		renderQueue.Flush(context, gpuConstants);
	}
	renderQueue.Clear();

	// シャドウマップを次のフレームでまた書き込めるように戻す
	D3D12_RESOURCE_BARRIER srvToShadow = CD3DX12_RESOURCE_BARRIER::Transition(
		shadowMap->Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_DEPTH_WRITE);
	context.ResourceBarrier(1, &srvToShadow);

	gpuConstants.Reset();
}

void Application::ReportProfile()
{
	// 起動処理と最初のフレーム(テクスチャ転送などが集中する)
	if (frameIndex == 0)
	{
		AddMemoryUsageNotes();
		Profiler::Report("Startup profile (Initialize + first frame)");
		Profiler::SetEnabled(false);
	}

	// 起動直後の不安定な時期を避け、定常状態のフレームを平均する
	constexpr int profileStartFrame = 120;
	constexpr int profileFrameCount = 120;

	if (frameIndex == profileStartFrame - 1)
	{
		Profiler::SetEnabled(true);
		profileStartTime = std::chrono::steady_clock::now();
	}
	else if (frameIndex == profileStartFrame + profileFrameCount - 1)
	{
		const std::chrono::duration<double> elapsed = std::chrono::steady_clock::now() - profileStartTime;
		Profiler::AddNote("FPS: " + std::to_string(profileFrameCount / elapsed.count()));
		Profiler::Report("Frame profile (" + std::to_string(profileFrameCount) + " frames)");
		Profiler::SetEnabled(false);
	}

	frameIndex++;
}

void Application::AddMemoryUsageNotes() const
{
	// プロセスが確保しているメモリ (CPU側)
	PROCESS_MEMORY_COUNTERS_EX processMemory = {};
	if (GetProcessMemoryInfo(GetCurrentProcess(), reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&processMemory), sizeof(processMemory)))
	{
		Profiler::AddNote("Process private memory: " + std::to_string(processMemory.PrivateUsage / (1024 * 1024)) + " MB");
	}

	// デバイスを作ったアダプタのVRAM使用量
	winrt::com_ptr<IDXGIFactory4> factory;
	if (FAILED(CreateDXGIFactory1(IID_PPV_ARGS_WRT(factory))))
		return;

	winrt::com_ptr<IDXGIAdapter3> adapter;
	if (FAILED(factory->EnumAdapterByLuid(renderPipeline.GetDevice()->GetAdapterLuid(), IID_PPV_ARGS_WRT(adapter))))
		return;

	DXGI_ADAPTER_DESC2 desc = {};
	adapter->GetDesc2(&desc);
	char adapterName[128] = {};
	WideCharToMultiByte(CP_UTF8, 0, desc.Description, -1, adapterName, sizeof(adapterName), nullptr, nullptr);
	Profiler::AddNote(std::string("Adapter: ") + adapterName + " (dedicated VRAM " + std::to_string(desc.DedicatedVideoMemory / (1024 * 1024)) + " MB)");

	// LOCAL = GPU専用メモリ, NON_LOCAL = GPUから使うシステムメモリ(共有メモリ)
	DXGI_QUERY_VIDEO_MEMORY_INFO local = {};
	DXGI_QUERY_VIDEO_MEMORY_INFO nonLocal = {};
	adapter->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &local);
	adapter->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_NON_LOCAL, &nonLocal);

	Profiler::AddNote("VRAM (local):     usage " + std::to_string(local.CurrentUsage / (1024 * 1024)) + " MB / budget " + std::to_string(local.Budget / (1024 * 1024)) + " MB");
	Profiler::AddNote("Shared (non-local): usage " + std::to_string(nonLocal.CurrentUsage / (1024 * 1024)) + " MB / budget " + std::to_string(nonLocal.Budget / (1024 * 1024)) + " MB");
}
