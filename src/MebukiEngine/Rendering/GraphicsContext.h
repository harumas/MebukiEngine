#pragma once

class GraphicsContext
{
public:
	// コンストラクタでコマンドリストを受け取る
	explicit GraphicsContext(const winrt::com_ptr<ID3D12GraphicsCommandList>& cmdList, const WindowInfo& windowInfo) :
		commandList(cmdList),
		aspectRatio(static_cast<float>(windowInfo.width) / static_cast<float>(windowInfo.height))
	{
	}

	float GetAspectRatio() const;
	void SetGraphicsRootConstantBufferView(UINT rootParameter, D3D12_GPU_VIRTUAL_ADDRESS address) const;
	void SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY topology) const;
	void SetIndexBuffer(const D3D12_INDEX_BUFFER_VIEW& indexBufferView) const;
	void SetVertexBuffer(UINT startSlot, const D3D12_VERTEX_BUFFER_VIEW& vertexBufferView) const;
	void Draw(UINT vertexCount, UINT vertexStartOffset) const;
	void DrawInstanced(UINT vertexCountPerInstance, UINT instanceCount, UINT startVertexLocation, UINT startInstanceLocation) const;
	void SetPipelineState(ID3D12PipelineState* pipelineState) const;

private:
	winrt::com_ptr<ID3D12GraphicsCommandList> commandList;
	float aspectRatio;

	GraphicsContext(const GraphicsContext&) = delete;
	GraphicsContext& operator=(const GraphicsContext&) = delete;
};
