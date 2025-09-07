#pragma once
#include "GraphicsContext.h"
#include "GraphicsDevice.h"

template <typename T>
class ConstantBuffer
{
public:
	ConstantBuffer();

	void SetBufferData(const T& buffer);
	void SetConstantBufferView(const GraphicsContext& context, UINT rootParameter) const;

private:
	winrt::com_ptr<ID3D12Resource> constantBuffer;
};

template <typename T>
ConstantBuffer<T>::ConstantBuffer()
{
	// 定数バッファーの生成
	const auto constHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	constexpr auto size = (sizeof(T) + 0xff) & ~0xff;
	const CD3DX12_RESOURCE_DESC constDesc = CD3DX12_RESOURCE_DESC::Buffer(size); // 256アライメントでサイズを指定

	winrt::check_hresult(GraphicsDevice::Get()->CreateCommittedResource(
		&constHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&constDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS_WRT(constantBuffer)));
}

template <typename T>
void ConstantBuffer<T>::SetBufferData(const T& buffer)
{
	//バッファに書き込む
	T* constMap = nullptr;
	const CD3DX12_RANGE readRange(0, 0);

	if (SUCCEEDED(constantBuffer->Map(0, &readRange, reinterpret_cast<void**>(&constMap))))
	{
		memcpy_s(constMap, sizeof(T), &buffer, sizeof(T));
		constantBuffer->Unmap(0, nullptr);
	}
}

template <typename T>
void ConstantBuffer<T>::SetConstantBufferView(const GraphicsContext& context, UINT rootParameter) const
{
	context.SetGraphicsRootConstantBufferView(rootParameter, constantBuffer->GetGPUVirtualAddress());
}


