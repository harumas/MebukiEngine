#include "Texture.h"

#include "Rendering/GraphicsContext.h"
#include "Basic/Profiler.h"

void Texture::Create(const GraphicsContext& context, const std::wstring& path)
{
	DirectX::ScratchImage scratch;
	DirectX::TexMetadata metadata;

	// 画像ファイルを読み込んでデコード
	{
		ScopedTimer timer("  texture: decode");
		// FORCE_RGB:    BGRAではなくRGBAでデコードさせる
		// IGNORE_SRGB:  PNGにsRGBチャンクがあると_SRGB形式になり、UNORMへのConvertでガンマ変換が走ってしまう
		// どちらもConvertを避けるため (DebugのDirectXTexでは1枚150msかかっていた)
		winrt::check_hresult(DirectX::LoadFromWICFile(
			path.c_str(),
			DirectX::WIC_FLAGS_FORCE_RGB | DirectX::WIC_FLAGS_IGNORE_SRGB,
			&metadata,
			scratch
		));
	}

	// FORCE_RGBでも16bit PNGなどはRGBA8にならないので、その場合だけ変換する
	// (同じフォーマットへのConvertは失敗するため、無条件には呼べない)
	if (metadata.format != DXGI_FORMAT_R8G8B8A8_UNORM)
	{
		DirectX::ScratchImage convertedScratch;

		{
			ScopedTimer timer("  texture: convert to RGBA");
			winrt::check_hresult(DirectX::Convert(
				scratch.GetImages(),        // 元の画像
				scratch.GetImageCount(),
				metadata,
				DXGI_FORMAT_R8G8B8A8_UNORM, // 変換先フォーマット
				DirectX::TEX_FILTER_DEFAULT,
				0.0f,
				convertedScratch
			));
		}

		CreateUploadResources(context, convertedScratch, convertedScratch.GetMetadata());
		return;
	}

	// GPUに転送するためのリソースを作成
	CreateUploadResources(context, scratch, metadata);
}

void Texture::Create(const GraphicsContext& context, const std::vector<uint8_t>& byteData)
{
	DirectX::TexMetadata metadata;
	DirectX::ScratchImage scratch;

	// WICを使ってメモリ上のPNG/JPEGデータをデコード
	{
		ScopedTimer timer("  texture: decode");
		winrt::check_hresult(DirectX::LoadFromWICMemory(
			byteData.data(),
			byteData.size(),                  // PNG/JPEGのバイト列
			DirectX::WIC_FLAGS_FORCE_RGB | DirectX::WIC_FLAGS_IGNORE_SRGB, // 理由はパス指定版のCreateを参照
			&metadata, scratch                  // 出力
		));
	}

	// SRVはR8G8B8A8で作るので、PNGの種類によってBGRA等でデコードされた場合は変換する
	if (metadata.format != DXGI_FORMAT_R8G8B8A8_UNORM)
	{
		DirectX::ScratchImage convertedScratch;

		{
			ScopedTimer timer("  texture: convert to RGBA");
			winrt::check_hresult(DirectX::Convert(
				scratch.GetImages(),
				scratch.GetImageCount(),
				metadata,
				DXGI_FORMAT_R8G8B8A8_UNORM,
				DirectX::TEX_FILTER_DEFAULT,
				0.0f,
				convertedScratch
			));
		}

		CreateUploadResources(context, convertedScratch, convertedScratch.GetMetadata());
		return;
	}

	// GPUに転送するためのリソースを作成
	CreateUploadResources(context, scratch, metadata);
}

ID3D12Resource* Texture::GetTextureResource()
{
	return textureResource.get();
}

void Texture::CreateUploadResources(const GraphicsContext& context, const DirectX::ScratchImage& scratch, const DirectX::TexMetadata& metadata)
{
	ScopedTimer timer("  texture: create GPU resource + upload");

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
	winrt::check_hresult(context.GetDevice()->CreateCommittedResource(
		&heapProps, // GPU用
		D3D12_HEAP_FLAG_NONE,
		&texDesc,
		D3D12_RESOURCE_STATE_COPY_DEST, // コピー先にする
		nullptr,
		IID_PPV_ARGS_WRT(textureResource)
	));

	std::vector<D3D12_SUBRESOURCE_DATA> subResources;

	// サブリソース情報を用意
	winrt::check_hresult(DirectX::PrepareUpload(context.GetDevice(), scratch.GetImages(), scratch.GetImageCount(),
		scratch.GetMetadata(), subResources));


	UINT64 uploadBufferSize = GetRequiredIntermediateSize(textureResource.get(), 0, static_cast<UINT>(subResources.size()));

	heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	texDesc = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);

	// アップロード用バッファの生成
	winrt::check_hresult(context.GetDevice()->CreateCommittedResource(
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
