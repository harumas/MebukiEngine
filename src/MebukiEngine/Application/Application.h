#pragma once
#include "Rendering/GpuConstants.h"
#include "Rendering/RenderPipeline.h"
#include "Rendering/RenderQueue.h"
#include "Toolkit/World/WorldManager.h"
#include "Toolkit/World/WorldProfile.h"
#include "Toolkit/Time/Time.h"
#include "ShadowMap.h"
#include "Toolkit/Rendering/ShadowPass.h"
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
	void Resize(UINT width, UINT height);
	void Finalize();

private:
	bool isInitialized = false;
	int frameIndex = 0; // 計測するフレームを決めるためのカウンタ
	RenderPipeline renderPipeline;
	WorldManager worldManager;
	EngineService engineService;
	InputProvider inputProvider;
	Time time;
	RenderQueue renderQueue;
	WindowInfo currentWindowInfo = {};
	std::unique_ptr<ShadowMap> shadowMap;
	std::shared_ptr<ShadowPass> shadowShaderPass;
	std::shared_ptr<ActorService> actorService;

	void Render(const GraphicsContext& context, GpuConstants& gpuConstants);

	// 起動時と、定常状態の一定フレームの計測結果を出力する
	void ReportProfile();

	// 起動時の計測結果に、メモリとVRAMの使用量を添える
	void AddMemoryUsageNotes() const;

	std::chrono::steady_clock::time_point profileStartTime;
};

