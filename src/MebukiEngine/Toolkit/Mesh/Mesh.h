#pragma once
#include "MeshData.h"
#include <DirectXTex.h>

struct ImageData
{
	DirectX::TexMetadata metadata;
	DirectX::ScratchImage scratch;
	const DirectX::Image* image;
};

class Mesh
{
public:
	Mesh();
	Mesh(ID3D12Device* device, const std::string& path, D3D12_PRIMITIVE_TOPOLOGY topology);
	Mesh(ID3D12Device* device, std::vector<Vertex> vertices, std::vector<uint16_t> indices, D3D12_PRIMITIVE_TOPOLOGY topology);

	// glTFに含まれる全プリミティブを、それぞれ独立したMeshとして読み込む
	static std::vector<std::shared_ptr<Mesh>> LoadAll(ID3D12Device* device, const std::string& path, D3D12_PRIMITIVE_TOPOLOGY topology);

	bool HasTexture() const;
	bool IsUploaded() const;
	MeshData& GetMeshData() { return meshData; }
	D3D12_VERTEX_BUFFER_VIEW& GetVertexBufferView() { return vertexBufferView; }
	D3D12_INDEX_BUFFER_VIEW& GetIndexBufferView() { return indexBufferView; }
	UINT GetIndexCount() const { return indexCount; }

	void RecordUpload(ID3D12GraphicsCommandList* commandList);

	void TickUploadBufferRelease();

private:
	MeshData meshData = {};
	bool isUploaded = false;

	// 何フレーム後にUploadBufferをリリースするか 
	int framesUntilUploadBufferRelease = -1;

	winrt::com_ptr<ID3D12Resource> vertexBuffer = nullptr;
	winrt::com_ptr<ID3D12Resource> indexBuffer = nullptr;

	winrt::com_ptr<ID3D12Resource> vertexUploadBuffer = nullptr;
	winrt::com_ptr<ID3D12Resource> indexUploadBuffer = nullptr;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView = {};
	D3D12_INDEX_BUFFER_VIEW indexBufferView = {};
	UINT indexCount = 0;

	// meshDataに入っている内容からGPUバッファを作る
	void CreateBuffers(ID3D12Device* device);
	void CreateVertexBuffer(ID3D12Device* device, const std::vector<Vertex>& vertices);
	void CreateIndexBuffer(ID3D12Device* device, const void* indices, size_t count, bool use32bit);

	// GPUバッファへ転送し終えた頂点・インデックスをCPU側から解放する
	void ReleaseCpuData();
};
