#include "Texture.h"

#include "Rendering/GraphicsContext.h"
#include "Rendering/GraphicsDevice.h"

void Texture::Create(const GraphicsContext& context, const std::wstring& path)
{
	DirectX::ScratchImage scratch;
	DirectX::TexMetadata metadata;

	// 画像ファイルを読み込んでデコード
	winrt::check_hresult(DirectX::LoadFromWICFile(
		path.c_str(),
		DirectX::WIC_FLAGS_NONE,
		&metadata,
		scratch
	));

	DirectX::ScratchImage convertedScratch;

	// RGBAに変換
	DirectX::Convert(
		scratch.GetImages(),        // 元の画像
		scratch.GetImageCount(),
		metadata,
		DXGI_FORMAT_R8G8B8A8_UNORM, // 変換先フォーマット
		DirectX::TEX_FILTER_DEFAULT,
		0.0f,
		convertedScratch
	);

	metadata = convertedScratch.GetMetadata();

	// GPUに転送するためのリソースを作成
	CreateUploadResources(context, convertedScratch, metadata);
}

void Texture::Create(const GraphicsContext& context, const std::vector<uint8_t>& byteData)
{
	DirectX::TexMetadata metadata;
	DirectX::ScratchImage scratch;

	// WICを使ってメモリ上のPNG/JPEGデータをデコード
	winrt::check_hresult(DirectX::LoadFromWICMemory(
		byteData.data(),
		byteData.size(),                  // PNG/JPEGのバイト列
		DirectX::WIC_FLAGS_NONE,            // 読み込みフラグ
		&metadata, scratch                  // 出力
	));

	// GPUに転送するためのリソースを作成
	CreateUploadResources(context, scratch, metadata);
}

ID3D12Resource* Texture::GetTextureResource()
{
	return textureResource.get();
}

void Texture::CreateUploadResources(const GraphicsContext& context, const DirectX::ScratchImage& scratch, const DirectX::TexMetadata& metadata)
{
	// デコード済みの画像データを取得
	const DirectX::Image* image = scratch.GetImage(0, 0, 0);

	CD3DX12_RESOURCE_DESC texDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		image->format,          // フォーマット (例: DXGI_FORMAT_R8G8B8A8_UNORM)
		image->width,
		image->height,
		static_cast<UINT16>(metadata.arraySize),
		static_cast<UINT16>(metadata.mipLevels)
	);

	auto heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

	// テクスチャ用リソースの生成
	winrt::check_hresult(GraphicsDevice::Get()->CreateCommittedResource(
		&heapProps, // GPU用
		D3D12_HEAP_FLAG_NONE,
		&texDesc,
		D3D12_RESOURCE_STATE_COPY_DEST, // コピー先にする
		nullptr,
		IID_PPV_ARGS_WRT(textureResource)
	));

	std::vector<D3D12_SUBRESOURCE_DATA> subResources;

	// サブリソース情報を用意
	winrt::check_hresult(DirectX::PrepareUpload(GraphicsDevice::Get(), scratch.GetImages(), scratch.GetImageCount(),
		scratch.GetMetadata(), subResources));


	UINT64 uploadBufferSize = GetRequiredIntermediateSize(textureResource.get(), 0, static_cast<UINT>(subResources.size()));

	heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	texDesc = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);

	// アップロード用バッファの生成
	winrt::check_hresult(GraphicsDevice::Get()->CreateCommittedResource(
		&heapProps, // CPU→GPU転送用
		D3D12_HEAP_FLAG_NONE,
		&texDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS_WRT(uploadHeap)
	));

	// UpdateSubresourcesでコピー命令を積む
	UpdateSubresources(context.GetCommandList(),
		textureResource.get(),
		uploadHeap.get(),
		0,
		0,
		static_cast<UINT>(subResources.size()),
		subResources.data());

	// テクスチャをシェーダーで使えるように戻す
	CD3DX12_RESOURCE_BARRIER transition = CD3DX12_RESOURCE_BARRIER::Transition(
		textureResource.get(),
		D3D12_RESOURCE_STATE_COPY_DEST,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	context.ResourceBarrier(1, &transition);
}
