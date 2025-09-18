#pragma once
#include "GraphicsContext.h"
#include "GraphicsDevice.h"

template <class T>
class ConstantBuffer
{
public:
	explicit ConstantBuffer(UINT rootParameter, UINT count);
	~ConstantBuffer();

	// バッファをGPUに転送します
	void UploadBufferData(const void* src) const;

	// 指定されたサイズとオフセットのバッファをGPUに転送します
	void UploadBufferData(const void* src, size_t size, size_t offset);

	// ルートパラメータに定数バッファビューを設定します
	void SetRootConstantBufferView(const GraphicsContext& context, uint32_t offset = 0) const;

private:
	winrt::com_ptr<ID3D12Resource> constantBuffer;
	T* mappedBuffer;
	UINT rootParameter;
	UINT count;
	size_t bufferSize;
};

template <class T>
ConstantBuffer<T>::ConstantBuffer(UINT rootParameter, UINT count) :
	rootParameter(rootParameter),
	count(count),
	bufferSize(sizeof(T)* count)
{
	// Tのサイズが256バイトアラインされていることを確認
	if (sizeof(T) % 256 != 0)
	{
		throw std::invalid_argument("ConstantBuffer: Size of T must be 256-byte aligned");
	}

	// 定数バッファーの生成
	const auto constHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	const CD3DX12_RESOURCE_DESC constDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize); // 256アライメントでサイズを指定

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

template <class T>
ConstantBuffer<T>::~ConstantBuffer()
{
	constantBuffer->Unmap(0, nullptr);
}

template <class T>
void ConstantBuffer<T>::UploadBufferData(const void* src) const
{
	memcpy_s(mappedBuffer, bufferSize, src, bufferSize);
}

template <class T>
void ConstantBuffer<T>::UploadBufferData(const void* src, size_t size, size_t offset)
{
	if (offset + size > bufferSize)
	{
		throw std::out_of_range("ConstantBuffer: Upload size exceeds buffer size");
	}

	size_t addressOffset = sizeof(T) * offset;

	//指定されたオフセットとサイズでコピーする 
	uint8_t* dst = reinterpret_cast<uint8_t*>(mappedBuffer);
	memcpy_s(dst + addressOffset, size, src, size);
}

template <class T>
void ConstantBuffer<T>::SetRootConstantBufferView(const GraphicsContext& context, uint32_t offset) const
{
	size_t addressOffset = sizeof(T) * offset;
	D3D12_GPU_VIRTUAL_ADDRESS address = constantBuffer->GetGPUVirtualAddress() + addressOffset;
	context.SetGraphicsRootConstantBufferView(rootParameter, address);
}

