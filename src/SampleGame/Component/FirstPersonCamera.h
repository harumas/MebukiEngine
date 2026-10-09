#pragma once
#include <Toolkit/Component/Component.h>
#include <Toolkit/Component/Transform.h>
#include <Toolkit/Component/Camera.h>
#include "Basic/EngineService.h"
#include "Rendering/RenderPipeline.h"

// WASDで移動、右ドラッグで視点を回すフリーカメラ。左上に現在の座標を表示する
class FirstPersonCamera : public Camera
{
public:
	explicit FirstPersonCamera(ActorRef actorRef);

	void Setup(const EngineService& engineService);
	void OnUpdate(float deltaTime) override;
	void OnDestroy() override;

private:
	// onPostRenderProcess にバインドされる座標表示のコールバック
	void OnPostRender(const GraphicsContext& context, GpuConstants& gpuConstants);

	void UpdateRotation();
	void UpdatePosition(float deltaTime);

	RenderPipeline* renderPipeline = nullptr;
	UINT postRenderHandle = static_cast<UINT>(-1);
};
