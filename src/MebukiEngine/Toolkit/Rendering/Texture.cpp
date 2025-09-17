#include "Texture.h"

#include "Rendering/GraphicsContext.h"

void Texture::SetResources(winrt::com_ptr<ID3D12Resource> tex, winrt::com_ptr<ID3D12Resource> upload,
	std::vector<D3D12_SUBRESOURCE_DATA> subs)
{
	textureResource = std::move(tex);
	uploadHeap = std::move(upload);
	subResources = std::move(subs);
}

void Texture::UpdateSubResources(const GraphicsContext& context)
{
	// テクスチャを COPY_DEST に切り替え
	CD3DX12_RESOURCE_BARRIER transition = CD3DX12_RESOURCE_BARRIER::Transition(
		textureResource.get(),
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_COPY_DEST);

	context.ResourceBarrier(1, &transition);

	// UpdateSubresourcesでコピー命令を積む
	UpdateSubresources(context.GetCommandList(),
		textureResource.get(),
		uploadHeap.get(),
		0,
		0,
		static_cast<UINT>(subResources.size()), subResources.data());

	// テクスチャをシェーダーで使えるように戻す
	transition = CD3DX12_RESOURCE_BARRIER::Transition(
		textureResource.get(),
		D3D12_RESOURCE_STATE_COPY_DEST,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	context.ResourceBarrier(1, &transition);
}

ID3D12Resource* Texture::GetTextureResource()
{
	return textureResource.get();
}
