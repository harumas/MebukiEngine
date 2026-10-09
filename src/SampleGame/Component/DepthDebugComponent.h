#pragma once
#include "Toolkit/Component/Component.h"
#include "Toolkit/Rendering/Material.h"
#include "Rendering/RenderPipeline.h"
#include "Basic/EngineService.h"

class DepthDebugComponent : public Component
{
public:
	explicit DepthDebugComponent(ActorRef actorRef);
	~DepthDebugComponent() override = default;

	void Setup(const EngineService& engineService);
	void OnDestroy() override;

private:
	void PostRender(const GraphicsContext& context, GpuConstants& gpuConstants);

	std::shared_ptr<Material> debugDepthMaterial;
	RenderPipeline* renderPipeline = nullptr;
	UINT depthSrvHandle = static_cast<UINT>(-1);
	UINT postRenderHandle = static_cast<UINT>(-1);
};
