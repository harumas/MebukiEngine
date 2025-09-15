#include "GraphicsContext.h"

float GraphicsContext::GetAspectRatio() const
{
	return aspectRatio;
}

void GraphicsContext::SetGraphicsRootConstantBufferView(UINT rootParameter, D3D12_GPU_VIRTUAL_ADDRESS address) const
{
	commandList->SetGraphicsRootConstantBufferView(rootParameter, address);
}

void GraphicsContext::SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY topology) const
{
	commandList->IASetPrimitiveTopology(topology);
}

void GraphicsContext::SetIndexBuffer(const D3D12_INDEX_BUFFER_VIEW& indexBufferView) const
{
	commandList->IASetIndexBuffer(&indexBufferView);
}

void GraphicsContext::SetVertexBuffer(UINT startSlot, const D3D12_VERTEX_BUFFER_VIEW& vertexBufferView) const
{
	commandList->IASetVertexBuffers(startSlot, 1, &vertexBufferView);
}

void GraphicsContext::Draw(UINT indexCount, UINT indexStartOffset) const
{
	commandList->DrawIndexedInstanced(indexCount, 1, indexStartOffset, 0, 0);
}

void GraphicsContext::DrawInstanced(UINT indexCountPerInstance, UINT instanceCount, UINT startIndexLocation,
	UINT startInstanceLocation) const
{
	commandList->DrawIndexedInstanced(indexCountPerInstance, instanceCount, startIndexLocation, 0, startInstanceLocation);
}

void GraphicsContext::SetPipelineState(ID3D12PipelineState* pipelineState) const
{
	commandList->SetPipelineState(pipelineState);
}