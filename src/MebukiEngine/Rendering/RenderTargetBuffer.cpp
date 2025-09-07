#include "RenderTargetBuffer.h"

RenderTargetBuffer::RenderTargetBuffer(ID3D12Device* device, IDXGISwapChain3* swapChain, UINT frameBufferCount) :
	device(device),
	swapChain(swapChain),
	frameBufferCount(frameBufferCount)
{
	renderTargets.resize(frameBufferCount);

	backBufferIndex = swapChain->GetCurrentBackBufferIndex();

	// レンダーターゲットバッファーの生成
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
	rtvHeapDesc.NumDescriptors = frameBufferCount;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	winrt::check_hresult(device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS_WRT(rtvHeap)));

	// スワップチェーンと関連付けてレンダーターゲットビューを生成
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart());

	//フレームごとのRTVを作成 
	for (UINT i = 0; i < frameBufferCount; ++i)
	{
		winrt::check_hresult(swapChain->GetBuffer(i, IID_PPV_ARGS_WRT(renderTargets[i])));
		device->CreateRenderTargetView(renderTargets[i].get(), nullptr, rtvHandle);
		rtvHandle.Offset(1, device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
	}
}

D3D12_RESOURCE_BARRIER RenderTargetBuffer::GetResourceBarrier(D3D12_RESOURCE_STATES stateBefore, D3D12_RESOURCE_STATES stateAfter) const
{
	ID3D12Resource* renderTarget = renderTargets[backBufferIndex].get();
	const CD3DX12_RESOURCE_BARRIER resourceBarrier(CD3DX12_RESOURCE_BARRIER::Transition(renderTarget, stateBefore, stateAfter));
	return resourceBarrier;
}

D3D12_CPU_DESCRIPTOR_HANDLE RenderTargetBuffer::GetRTVHandle() const
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(
		rtvHeap->GetCPUDescriptorHandleForHeapStart(),
		backBufferIndex,
		device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));

	return rtvHandle;
}

UINT RenderTargetBuffer::GetBackBufferIndex() const
{
	return backBufferIndex;
}

void RenderTargetBuffer::RefreshBackBufferIndex()
{
	backBufferIndex = swapChain->GetCurrentBackBufferIndex();
}

