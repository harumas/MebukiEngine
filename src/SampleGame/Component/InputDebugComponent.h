#pragma once
#include "Toolkit/Component/Component.h"
#include "Basic/EngineService.h"
#include "Rendering/RenderPipeline.h"

// キーボードとマウスの入力状態を ImGui ウィンドウで表示するコンポーネント
class InputDebugComponent : public Component
{
public:
	explicit InputDebugComponent(ActorRef actorRef);
	~InputDebugComponent() override = default;

	void Setup(const EngineService& engineService);
	void OnDestroy() override;

private:
	// onPostRenderProcess にバインドされる描画コールバック
	void OnPostRender(const GraphicsContext& context, GpuConstants& gpuConstants);

	RenderPipeline* renderPipeline = nullptr;
	UINT postRenderHandle = static_cast<UINT>(-1);
};
