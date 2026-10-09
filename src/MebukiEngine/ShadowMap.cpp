#include "ShadowMap.h"
#include "Rendering/GpuConstants.h"

ShadowMap::ShadowMap(ID3D12Device* device, GpuConstants& gpuConstants, UINT width, UINT height) :
	width(width),
	height(height)
{
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	winrt::check_hresult(device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS_WRT(dsvHeap)));

	// バッファ作成
	D3D12_RESOURCE_DESC depthResDesc = {};
	depthResDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthResDesc.Width = width;
	depthResDesc.Height = height;
	depthResDesc.DepthOrArraySize = 1;
	depthResDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	depthResDesc.SampleDesc.Count = 1;
	depthResDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	depthResDesc.MipLevels = 1;
	depthResDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	depthResDesc.Alignment = 0;

	auto depthHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

	// クリアバリューの設定
	D3D12_CLEAR_VALUE _depthClearValue = {};
	_depthClearValue.DepthStencil.Depth = 1.0f;      //深さ１(最大値)でクリア
	_depthClearValue.Format = DXGI_FORMAT_D32_FLOAT; //32bit深度値としてクリア
	winrt::check_hresult(device->CreateCommittedResource(
		&depthHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&depthResDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE, //デプス書き込みに使用
		&_depthClearValue,
		IID_PPV_ARGS_WRT(shadowBuffer)));

	// 深度バッファービュー作成
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
	device->CreateDepthStencilView(shadowBuffer.get(), &dsvDesc, dsvHeap->GetCPUDescriptorHandleForHeapStart());

	// SRVのハンドルを保持しておく
	srvHandle = gpuConstants.CreateShaderResourceView(shadowBuffer.get(), DXGI_FORMAT_R32_FLOAT);
}

ID3D12Resource* ShadowMap::Get() const
{
	return shadowBuffer.get();
}

D3D12_CPU_DESCRIPTOR_HANDLE ShadowMap::GetDSVHandle() const
{
	return dsvHeap->GetCPUDescriptorHandleForHeapStart();
}

UINT ShadowMap::GetSRVHandle() const
{
	return srvHandle;
}

UINT ShadowMap::GetWidth() const
{
	return width;
}

UINT ShadowMap::GetHeight() const
{
	return height;
}
