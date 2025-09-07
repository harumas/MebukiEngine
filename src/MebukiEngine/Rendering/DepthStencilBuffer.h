#pragma once

class DepthStencilBuffer : public winrt::implements<DepthStencilBuffer, winrt::Windows::Foundation::IInspectable>
{
public:
	DepthStencilBuffer(ID3D12Device* device, UINT width, UINT height);
	ID3D12Resource* Get() const;
	D3D12_CPU_DESCRIPTOR_HANDLE GetDSVHandle() const;

private:
	winrt::com_ptr<ID3D12Resource> depthStencilBuffer = nullptr;
	winrt::com_ptr<ID3D12DescriptorHeap> dsvHeap = nullptr;
};
