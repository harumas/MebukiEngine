#pragma once
#include "Rendering/GraphicsContext.h"

class Texture
{
public:
	void SetResources(winrt::com_ptr<ID3D12Resource> tex, winrt::com_ptr<ID3D12Resource> upload, std::vector<D3D12_SUBRESOURCE_DATA> subs);
	void UpdateSubResources(const GraphicsContext& context);
	ID3D12Resource* GetTextureResource();

private:
	winrt::com_ptr<ID3D12Resource> textureResource = nullptr;
	winrt::com_ptr<ID3D12Resource> uploadHeap = nullptr;
	std::vector<D3D12_SUBRESOURCE_DATA> subResources;
};
