#pragma once
#include "GraphicsContext.h"
#include "GraphicsDevice.h"

template <typename T>
class ConstantBuffer
{
public:
	explicit ConstantBuffer(UINT rootParameter);
	~ConstantBuffer();

	// バッファをGPUに転送します
	void UploadBufferData(const T& buffer);

	// 指定されたサイズとオフセットのバッファをGPUに転送します
	void UploadBufferData(const void* src, size_t size, size_t offset);

	// ルートパラメータに定数バッファビューを設定します
	void SetRootConstantBufferView(const GraphicsContext& context, uint32_t offset = 0) const;

private:
	winrt::com_ptr<ID3D12Resource> constantBuffer;
	T* mappedBuffer;
	UINT rootParameter;
};

template <typename T>
ConstantBuffer<T>::ConstantBuffer(UINT rootParameter) : rootParameter(rootParameter)
{
	// 定数バッファーの生成
	const auto constHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	constexpr auto size = (sizeof(T) + 0xff) & ~0xff;
	const CD3DX12_RESOURCE_DESC constDesc = CD3DX12_RESOURCE_DESC::Buffer(size); // 256アライメントでサイズを指定

	// ヒープの生成
	winrt::check_hresult(GraphicsDevice::Get()->CreateCommittedResource(
		&constHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&constDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS_WRT(constantBuffer)));

	// 常にMapしておく (UPLOADタイプだけ)
	mappedBuffer = nullptr;
	const CD3DX12_RANGE readRange(0, 0);
	winrt::check_hresult(constantBuffer->Map(0, &readRange, reinterpret_cast<void**>(&mappedBuffer)));
}

template <typename T>
ConstantBuffer<T>::~ConstantBuffer()
{
	constantBuffer->Unmap(0, nullptr);
}


template <typename T>
void ConstantBuffer<T>::UploadBufferData(const T& buffer)
{
	memcpy_s(mappedBuffer, sizeof(T), &buffer, sizeof(T));
}

template <typename T>
void ConstantBuffer<T>::UploadBufferData(const void* src, size_t size, size_t offset)
{
	if (offset + size > sizeof(T))
	{
		throw std::out_of_range("ConstantBuffer: Upload size exceeds buffer size");
	}

	//指定されたオフセットとサイズでコピーする 
	uint8_t* dst = reinterpret_cast<uint8_t*>(mappedBuffer);
	memcpy_s(dst + offset, sizeof(T) - offset, src, size);
}

template <typename T>
void ConstantBuffer<T>::SetRootConstantBufferView(const GraphicsContext& context, uint32_t offset) const
{
	context.SetGraphicsRootConstantBufferView(rootParameter, constantBuffer->GetGPUVirtualAddress() + offset);
}




