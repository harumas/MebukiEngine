#pragma once
#include "ConstantBufferLayout.h"

class GraphicsContext
{
public:
	explicit GraphicsContext(const winrt::com_ptr<ID3D12GraphicsCommandList>& cmdList, const WindowInfo& windowInfo) :
		commandList(cmdList),
		aspectRatio(static_cast<float>(windowInfo.width) / static_cast<float>(windowInfo.height))
	{
	}

	// 画面のアスペクト比を取得します 
	float GetAspectRatio() const;

	// ルートシグネチャに定数バッファビューをセットします
	void SetGraphicsRootConstantBufferView(UINT rootParameter, D3D12_GPU_VIRTUAL_ADDRESS address) const;

	// プリミティブトポロジーをセットします
	void SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY topology) const;

	// 頂点バッファ、インデックスバッファをセットします
	void SetIndexBuffer(const D3D12_INDEX_BUFFER_VIEW& indexBufferView) const;
	void SetVertexBuffer(UINT startSlot, const D3D12_VERTEX_BUFFER_VIEW& vertexBufferView) const;

	// 描画コマンドを発行する関数群
	void Draw(UINT vertexCount, UINT vertexStartOffset) const;
	void DrawInstanced(UINT vertexCountPerInstance, UINT instanceCount, UINT startVertexLocation, UINT startInstanceLocation) const;

	// パイプラインステートオブジェクトをセットします
	void SetPipelineState(ID3D12PipelineState* pipelineState) const;

	// リソースバリアを設定します
	void ResourceBarrier(UINT numBarriers, const D3D12_RESOURCE_BARRIER* barriers) const;

	ID3D12GraphicsCommandList* GetCommandList() const;

private:
	winrt::com_ptr<ID3D12GraphicsCommandList> commandList;
	float aspectRatio;

	GraphicsContext(const GraphicsContext&) = delete;
	GraphicsContext& operator=(const GraphicsContext&) = delete;
};
