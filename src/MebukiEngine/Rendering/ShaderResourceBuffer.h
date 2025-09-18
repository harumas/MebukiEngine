#pragma once

class ShaderResourceBuffer
{
public:
	ShaderResourceBuffer(ID3D12Device* device);
	UINT CreateShaderResourceView(ID3D12Resource* resource, DXGI_FORMAT format);
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(UINT offset) const;
	ID3D12DescriptorHeap* GetDescriptorHeap() const;

private:
	winrt::com_ptr<ID3D12DescriptorHeap> srvHeap;
	UINT resourceCount;
};
