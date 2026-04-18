#pragma once
#include "Toolkit/Component/Component.h"
#include "Basic/EngineService.h"
#include "Rendering/RenderPipeline.h"

// キーボードとマウスの入力状態を ImGui ウィンドウで表示するコンポーネント
class InputDebugComponent : public Component
{
public:
	explicit InputDebugComponent(const std::shared_ptr<Actor>& actorRef);
	~InputDebugComponent() override = default;

	void Setup(const EngineService& engineService);

private:
	// onPostRenderProcess にバインドされる描画コールバック
	void OnPostRender(const GraphicsContext& context, GpuConstants& gpuConstants);
};
