#include "Mesh.h"
#include "ModelLoader.h"
#include "Rendering/GraphicsDevice.h"

Mesh::Mesh()
{

}

void Mesh::Create(const std::string& path, D3D12_PRIMITIVE_TOPOLOGY topology)
{
	ModelLoader loader;
	meshData = loader.Load(path, topology);

	//頂点バッファの作成 
	CreateVertexBuffer(meshData.vertices);

	//インデックスバッファの作成
	const void* indicesData = meshData.use32bitIndex
		? static_cast<const void*>(meshData.indices32.data())
		: static_cast<const void*>(meshData.indices16.data());
	size_t indicesCount = meshData.use32bitIndex ? meshData.indices32.size() : meshData.indices16.size();
	CreateIndexBuffer(indicesData, indicesCount, meshData.use32bitIndex);

	// テクスチャの作成 
	CreateTexture(meshData.texHandle.dataPtr, meshData.texHandle.dataSize);
}
void Mesh::Create(std::vector<Vertex> vertices, std::vector<uint16_t> indices, D3D12_PRIMITIVE_TOPOLOGY topology)
{
	meshData.vertices = std::move(vertices);
	meshData.indices16 = std::move(indices);
	meshData.use32bitIndex = false;
	meshData.topology = topology;

	CreateVertexBuffer(meshData.vertices);
	CreateIndexBuffer(meshData.indices16.data(), meshData.indices16.size(), false);
}

bool Mesh::HasTexture() const
{
	return meshData.texHandle.dataPtr != nullptr;
}

void Mesh::CreateVertexBuffer(const std::vector<Vertex>& vertices)
{
	// 頂点座標
	const UINT vertexBufferSize = sizeof(Vertex) * vertices.size();
	auto vertexHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	auto vertexResDesc = CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize);

	// 頂点バッファーの生成
	winrt::check_hresult(GraphicsDevice::Get()->CreateCommittedResource(
		&vertexHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&vertexResDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS_WRT(vertexBuffer)));

	// 頂点情報のコピー
	Vertex* vertexMap = nullptr;
	ThrowIfFailed(vertexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&vertexMap)));
	std::ranges::copy(vertices, vertexMap);
	vertexBuffer->Unmap(0, nullptr);

	// 頂点バッファービューの生成
	vertexBufferView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
	vertexBufferView.StrideInBytes = sizeof(Vertex);
	vertexBufferView.SizeInBytes = vertexBufferSize;
}

void Mesh::CreateIndexBuffer(const void* indices, size_t count, bool use32bit)
{
	const UINT indexSize = use32bit ? sizeof(uint32_t) : sizeof(uint16_t);
	const UINT indexBufferSize = static_cast<UINT>(count * indexSize);

	auto indexHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	auto indexResDesc = CD3DX12_RESOURCE_DESC::Buffer(indexBufferSize);

	winrt::check_hresult(GraphicsDevice::Get()->CreateCommittedResource(
		&indexHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&indexResDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS_WRT(indexBuffer)));

	// コピー
	void* mapped = nullptr;
	indexBuffer->Map(0, nullptr, &mapped);
	std::memcpy(mapped, indices, indexBufferSize);
	indexBuffer->Unmap(0, nullptr);

	// IBV設定
	indexBufferView.BufferLocation = indexBuffer->GetGPUVirtualAddress();
	indexBufferView.SizeInBytes = indexBufferSize;
	indexBufferView.Format = use32bit ? DXGI_FORMAT_R32_UINT : DXGI_FORMAT_R16_UINT;
}

Texture Mesh::CreateTexture(const uint8_t* dataPtr, size_t dataSize)
{
	DirectX::TexMetadata metadata;
	DirectX::ScratchImage scratch;

	// WICを使ってメモリ上のPNG/JPEGデータをデコード
	winrt::check_hresult(DirectX::LoadFromWICMemory(
		dataPtr, dataSize,                  // PNG/JPEGのバイト列
		DirectX::WIC_FLAGS_NONE,            // 読み込みフラグ
		&metadata, scratch                  // 出力
	));

	// デコード済みの画像データを取得
	const DirectX::Image* image = scratch.GetImage(0, 0, 0);

	CD3DX12_RESOURCE_DESC texDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		image->format,          // フォーマット (例: DXGI_FORMAT_R8G8B8A8_UNORM)
		image->width,
		image->height,
		static_cast<UINT16>(metadata.arraySize),
		static_cast<UINT16>(metadata.mipLevels)
	);

	winrt::com_ptr<ID3D12Resource> textureResource = nullptr;
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

	UINT64 uploadBufferSize = GetRequiredIntermediateSize(textureResource.get(), 0, static_cast<UINT>(metadata.mipLevels));

	winrt::com_ptr<ID3D12Resource> uploadHeap;
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

	std::vector<D3D12_SUBRESOURCE_DATA> subResources;

	// サブリソース情報を用意
	winrt::check_hresult(DirectX::PrepareUpload(GraphicsDevice::Get(), scratch.GetImages(), scratch.GetImageCount(),
		scratch.GetMetadata(), subResources));

	meshData.textureData.SetResources(textureResource, uploadHeap, subResources);
}
