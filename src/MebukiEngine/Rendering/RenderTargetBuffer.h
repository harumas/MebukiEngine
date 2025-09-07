#pragma once

class RenderTargetBuffer : public winrt::implements<RenderTargetBuffer, winrt::Windows::Foundation::IInspectable>
{
public:
	RenderTargetBuffer(ID3D12Device* device, IDXGISwapChain3* swapChain, UINT frameBufferCount);
	D3D12_RESOURCE_BARRIER GetResourceBarrier(D3D12_RESOURCE_STATES stateBefore, D3D12_RESOURCE_STATES stateAfter) const;
	D3D12_CPU_DESCRIPTOR_HANDLE GetRTVHandle() const;
	UINT GetBackBufferIndex() const;
	void RefreshBackBufferIndex();

private:
	ID3D12Device* device;
	IDXGISwapChain3* swapChain;

	winrt::com_ptr<ID3D12DescriptorHeap> rtvHeap = nullptr;
	std::vector<winrt::com_ptr<ID3D12Resource>> renderTargets = {};
	UINT frameBufferCount;
	UINT backBufferIndex = 0;
};
