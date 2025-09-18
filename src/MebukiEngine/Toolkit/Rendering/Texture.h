#pragma once
#include "Rendering/GraphicsContext.h"

class Texture
{
public:
	void Create(const GraphicsContext& context, const std::wstring& path);
	void Create(const GraphicsContext& context, const std::vector<uint8_t>& byteData);
	ID3D12Resource* GetTextureResource();

private:
	void CreateUploadResources(const GraphicsContext& context, const DirectX::ScratchImage& scratch, const DirectX::TexMetadata& metadata);

	winrt::com_ptr<ID3D12Resource> textureResource = nullptr;
	winrt::com_ptr<ID3D12Resource> uploadHeap = nullptr;
};
