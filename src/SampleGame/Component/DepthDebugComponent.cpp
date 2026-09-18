#include "DepthDebugComponent.h"
#include "../ShaderPass/DebugDepthPass.h"
#include "Toolkit/Rendering/ShaderPassPool.h"
#include "Toolkit/Rendering/MaterialHandler.h"
#include <Rendering/GraphicsContext.h>
#include <Rendering/GpuConstants.h>

DepthDebugComponent::DepthDebugComponent(ActorRef actorRef) : Component(actorRef)
{}

void DepthDebugComponent::Setup(const EngineService& engineService)
{
	auto shaderPassPool = engineService.Resolve<ShaderPassPool>();
	auto materialHandler = engineService.Resolve<MaterialHandler>();

	renderPipeline = engineService.Resolve<RenderPipeline>().get();
	if (renderPipeline)
	{
		auto debugPass = shaderPassPool->GetShaderPass<DebugDepthPass>();
		debugDepthMaterial = std::make_shared<Material>(materialHandler->Create(debugPass));
		postRenderHandle = renderPipeline->onPostRenderProcess.AddListener(std::bind(&DepthDebugComponent::PostRender, this, std::placeholders::_1, std::placeholders::_2));
	}
}

void DepthDebugComponent::OnDestroy()
{
	if (renderPipeline && postRenderHandle != static_cast<UINT>(-1))
	{
		renderPipeline->onPostRenderProcess.RemoveListener(postRenderHandle);
	}
}

void DepthDebugComponent::PostRender(const GraphicsContext& context, GpuConstants& gpuConstants)
{
	if (!debugDepthMaterial || !renderPipeline) return;

	if (depthSrvHandle == static_cast<UINT>(-1))
	{
		auto depthBuffer = renderPipeline->GetDepthStencilBuffer();
		depthSrvHandle = gpuConstants.CreateShaderResourceView(depthBuffer->Get(), DXGI_FORMAT_R32_FLOAT);
	}

	debugDepthMaterial->SetPipelineState(context);
	gpuConstants.SetGraphicsRootDescriptorTable(context, depthSrvHandle);
	context.SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context.DrawNonIndexed(6, 1, 0, 0);
}
