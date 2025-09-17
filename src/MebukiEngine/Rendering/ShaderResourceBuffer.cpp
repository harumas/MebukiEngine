#include "ShaderResourceBuffer.h"
#include "GraphicsDevice.h"

ShaderResourceBuffer::ShaderResourceBuffer(ID3D12Device* device) :
	srvHeap(nullptr),
	resourceCount(0)
{
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.NumDescriptors = 128; // 必要な最大テクスチャ数
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	winrt::check_hresult(device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS_WRT(srvHeap)));
}

UINT ShaderResourceBuffer::CreateShaderResourceView(ID3D12Resource* resource, DXGI_FORMAT format)
{
	UINT descriptorSize = GraphicsDevice::Get()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// ヒープの先頭CPUハンドル
	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = srvHeap->GetCPUDescriptorHandleForHeapStart();

	// 例えば1番目のテクスチャを登録
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = format; // 例: DXGI_FORMAT_R8G8B8A8_UNORM
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 0;

	D3D12_CPU_DESCRIPTOR_HANDLE handle = cpuHandle;
	handle.ptr += resourceCount * descriptorSize; // スロット0番に書き込む

	GraphicsDevice::Get()->CreateShaderResourceView(resource, &srvDesc, handle);

	return resourceCount++;
}

D3D12_GPU_DESCRIPTOR_HANDLE ShaderResourceBuffer::GetGPUDescriptorHandle(UINT offset) const
{
	UINT descriptorSize = GraphicsDevice::Get()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = srvHeap->GetGPUDescriptorHandleForHeapStart();
	gpuHandle.ptr += offset * descriptorSize;

	return gpuHandle;
}

ID3D12DescriptorHeap* ShaderResourceBuffer::GetDescriptorHeap() const
{
	return srvHeap.get();
}


