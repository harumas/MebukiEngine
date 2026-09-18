#pragma once
class GpuConstants;

class ShadowMap
{
public:
	ShadowMap(ID3D12Device* device, GpuConstants& gpuConstants, UINT width, UINT height);
	ID3D12Resource* Get() const;
	D3D12_CPU_DESCRIPTOR_HANDLE GetDSVHandle() const;
	UINT GetSRVHandle() const;
	UINT GetWidth() const;
	UINT GetHeight() const;

private:
	winrt::com_ptr<ID3D12Resource> shadowBuffer = nullptr;
	winrt::com_ptr<ID3D12DescriptorHeap> dsvHeap = nullptr;
	UINT srvHandle = static_cast<UINT>(-1);
	UINT width;
	UINT height;
};
