#include "Mesh.h"



Mesh::Mesh()
{
}

void Mesh::Create(const std::wstring& path)
{
	// TODO: メッシュ読み込み処理の実装 
}

void Mesh::Create(const std::vector<Vertex>& vertices, const std::vector<unsigned short>& indices, D3D12_PRIMITIVE_TOPOLOGY topology)
{
	meshData.vertices = vertices;
	meshData.indices = indices;
	meshData.topology = topology;

	CreateVertexBuffer(vertices);
	CreateIndexBuffer(indices);
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

void Mesh::CreateIndexBuffer(const std::vector<unsigned short>& indices)
{
	// インデックス座標
	const UINT indexBufferSize = sizeof(unsigned short) * indices.size();
	auto indexHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	auto indexResDesc = CD3DX12_RESOURCE_DESC::Buffer(indexBufferSize);

	// インデックスバッファの生成
	winrt::check_hresult(GraphicsDevice::Get()->CreateCommittedResource(
		&indexHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&indexResDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS_WRT(indexBuffer)));

	// インデックス情報のコピー
	unsigned short* indexMap = nullptr;
	indexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&indexMap));
	std::ranges::copy(indices, indexMap);
	indexBuffer->Unmap(0, nullptr);

	// インデックスバッファビューの生成
	indexBufferView.BufferLocation = indexBuffer->GetGPUVirtualAddress();
	indexBufferView.SizeInBytes = indexBufferSize;
	indexBufferView.Format = DXGI_FORMAT_R16_UINT;	// 頂点座標
}
