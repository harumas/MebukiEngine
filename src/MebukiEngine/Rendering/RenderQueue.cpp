#include "RenderQueue.h"
#include "GraphicsContext.h"
#include "GpuConstants.h"
#include "Toolkit/Mesh/Mesh.h"
#include "Toolkit/Rendering/Material.h"

void RenderQueue::Submit(const DrawItem& item)
{
	items.push_back(item);
}

void RenderQueue::Flush(const GraphicsContext& context, const GpuConstants& gpuConstants) const
{
	for (const auto& item : items)
	{
		// 現在のTransformに定数バッファのオフセットを設定
		gpuConstants.SetTransformCBV(context, item.transformHandle);

		// 現在のMaterialに定数バッファのオフセットを設定
		gpuConstants.SetMaterialCBV(context, item.material->GetHandleId());

		// ShaderResourceViewのハンドルをセット
		if (item.shaderResourceHandle != static_cast<UINT>(-1))
		{
			gpuConstants.SetGraphicsRootDescriptorTable(context, item.shaderResourceHandle);
		}

		// PipelineStateを設定
		item.material->SetPipelineState(context);

		// トポロジー情報、頂点バッファ、インデックスバッファをIAステージに設定
		context.SetPrimitiveTopology(item.mesh->GetMeshData().topology);
		context.SetVertexBuffer(0, item.mesh->GetVertexBufferView());
		context.SetIndexBuffer(item.mesh->GetIndexBufferView());

		// 描画コマンドを発行
		context.Draw(item.mesh->GetIndexCount(), 0);
	}
}

void RenderQueue::FlushShadow(const GraphicsContext& context, const GpuConstants& gpuConstants) const
{
	for (const auto& item : items)
	{
		// 現在のTransformに定数バッファのオフセットを設定
		gpuConstants.SetTransformCBV(context, item.transformHandle);

		// トポロジー情報、頂点バッファ、インデックスバッファをIAステージに設定
		context.SetPrimitiveTopology(item.mesh->GetMeshData().topology);
		context.SetVertexBuffer(0, item.mesh->GetVertexBufferView());
		context.SetIndexBuffer(item.mesh->GetIndexBufferView());

		// 描画コマンドを発行 (マテリアル・PSOはシャドウパス側で共通のものが既にセットされている)
		context.Draw(item.mesh->GetIndexCount(), 0);
	}
}

void RenderQueue::Clear()
{
	items.clear();
}
