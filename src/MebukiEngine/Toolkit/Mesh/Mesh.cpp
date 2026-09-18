#include "Mesh.h"
#include "ModelLoader.h"
#include "Basic/Profiler.h"

Mesh::Mesh()
{}

Mesh::Mesh(ID3D12Device* device, const std::string& path, D3D12_PRIMITIVE_TOPOLOGY topology)
{
	ModelLoader loader;
	std::vector<MeshData> meshDataList = loader.Load(path, topology);

	if (meshDataList.empty())
	{
		ThrowMessage("メッシュが1つも含まれていません: " + path);
	}

	// 単一メッシュとして扱いたい呼び出し向けに、先頭のプリミティブだけを使う
	meshData = std::move(meshDataList[0]);
	CreateBuffers(device);
	ReleaseCpuData();
}

std::vector<std::shared_ptr<Mesh>> Mesh::LoadAll(ID3D12Device* device, const std::string& path, D3D12_PRIMITIVE_TOPOLOGY topology)
{
	ModelLoader loader;
	std::vector<MeshData> meshDataList = loader.Load(path, topology);

	std::vector<std::shared_ptr<Mesh>> meshes;
	meshes.reserve(meshDataList.size());

	for (MeshData& data : meshDataList)
	{
		auto mesh = std::make_shared<Mesh>();
		mesh->meshData = std::move(data);
		{
			ScopedTimer timer("  model: create vertex/index buffers");
			mesh->CreateBuffers(device);
		}
		{
			ScopedTimer timer("  model: release CPU data");
			mesh->ReleaseCpuData();
		}

		meshes.push_back(std::move(mesh));
	}

	return meshes;
}

void Mesh::CreateBuffers(ID3D12Device* device)
{
	//頂点バッファの作成
	CreateVertexBuffer(device, meshData.vertices);

	//インデックスバッファの作成
	const void* indicesData = meshData.use32bitIndex
		? static_cast<const void*>(meshData.indices32.data())
		: static_cast<const void*>(meshData.indices16.data());
	size_t indicesCount = meshData.use32bitIndex ? meshData.indices32.size() : meshData.indices16.size();
	CreateIndexBuffer(device, indicesData, indicesCount, meshData.use32bitIndex);
}

Mesh::Mesh(ID3D12Device* device, std::vector<Vertex> vertices, std::vector<uint16_t> indices, D3D12_PRIMITIVE_TOPOLOGY topology)
{
	meshData.vertices = std::move(vertices);
	meshData.indices16 = std::move(indices);
	meshData.use32bitIndex = false;
	meshData.topology = topology;

	CreateVertexBuffer(device, meshData.vertices);
	CreateIndexBuffer(device, meshData.indices16.data(), meshData.indices16.size(), false);
	ReleaseCpuData();
}

void Mesh::ReleaseCpuData()
{
	// バッファはUPLOADヒープにコピー済みで、描画にはインデックス数しか使わない
	// clear()だけでは確保済みの領域が残るので、空のvectorと入れ替えて解放する
	std::vector<Vertex>().swap(meshData.vertices);
	std::vector<uint16_t>().swap(meshData.indices16);
	std::vector<uint32_t>().swap(meshData.indices32);
}

bool Mesh::HasTexture() const
{
	return meshData.baseColorTexture != nullptr;
}

bool Mesh::IsUploaded() const
{
	return isUploaded;
}

void Mesh::CreateVertexBuffer(ID3D12Device* device, const std::vector<Vertex>& vertices)
{
	// 頂点座標
	const UINT vertexBufferSize = sizeof(Vertex) * vertices.size();
	auto vertexHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	auto vertexResDesc = CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize);

	// アップロード用頂点バッファーの生成
	winrt::check_hresult(device->CreateCommittedResource(
		&vertexHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&vertexResDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS_WRT(vertexUploadBuffer)));

	vertexHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

	// 頂点バッファーの生成
	winrt::check_hresult(device->CreateCommittedResource(
		&vertexHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&vertexResDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS_WRT(vertexBuffer)));

	// 頂点情報のコピー
	Vertex* vertexMap = nullptr;
	ThrowIfFailed(vertexUploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&vertexMap)));
	std::ranges::copy(vertices, vertexMap);
	vertexUploadBuffer->Unmap(0, nullptr);

	// 頂点バッファービューの生成
	vertexBufferView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
	vertexBufferView.StrideInBytes = sizeof(Vertex);
	vertexBufferView.SizeInBytes = vertexBufferSize;
}

void Mesh::CreateIndexBuffer(ID3D12Device* device, const void* indices, size_t count, bool use32bit)
{
	const UINT indexSize = use32bit ? sizeof(uint32_t) : sizeof(uint16_t);
	const UINT indexBufferSize = static_cast<UINT>(count * indexSize);

	auto indexHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	auto indexResDesc = CD3DX12_RESOURCE_DESC::Buffer(indexBufferSize);

	// アップロード用インデックスバッファの生成
	winrt::check_hresult(device->CreateCommittedResource(
		&indexHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&indexResDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS_WRT(indexUploadBuffer)));


	indexHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

	// インデックスバッファの生成
	winrt::check_hresult(device->CreateCommittedResource(
		&indexHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&indexResDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS_WRT(indexBuffer)));

	// コピー
	void* mapped = nullptr;
	indexUploadBuffer->Map(0, nullptr, &mapped);
	std::memcpy(mapped, indices, indexBufferSize);
	indexUploadBuffer->Unmap(0, nullptr);

	indexCount = static_cast<UINT>(count);

	// IBV設定
	indexBufferView.BufferLocation = indexBuffer->GetGPUVirtualAddress();
	indexBufferView.SizeInBytes = indexBufferSize;
	indexBufferView.Format = use32bit ? DXGI_FORMAT_R32_UINT : DXGI_FORMAT_R16_UINT;
}

void Mesh::RecordUpload(ID3D12GraphicsCommandList* commandList)
{
	UINT vertexBufferSize = vertexBufferView.SizeInBytes;
	commandList->CopyBufferRegion(vertexBuffer.get(), 0, vertexUploadBuffer.get(), 0, vertexBufferSize);

	UINT indexBufferSize = indexBufferView.SizeInBytes;
	commandList->CopyBufferRegion(indexBuffer.get(), 0, indexUploadBuffer.get(), 0, indexBufferSize);

	D3D12_RESOURCE_BARRIER barriers[2] =
	{
		CD3DX12_RESOURCE_BARRIER::Transition(
			vertexBuffer.get(),
			D3D12_RESOURCE_STATE_COPY_DEST,
			D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER),

		CD3DX12_RESOURCE_BARRIER::Transition(
			indexBuffer.get(),
			D3D12_RESOURCE_STATE_COPY_DEST,
			D3D12_RESOURCE_STATE_INDEX_BUFFER)
	};

	commandList->ResourceBarrier(2, barriers);

	isUploaded = true;
	framesUntilUploadBufferRelease = 3;
}

void Mesh::TickUploadBufferRelease()
{
	if (framesUntilUploadBufferRelease < 0)
		return;

	// UploadBufferをリリースする 
	if (--framesUntilUploadBufferRelease == 0)
	{
		vertexUploadBuffer = nullptr;
		indexUploadBuffer = nullptr;
		framesUntilUploadBufferRelease = -1;
	}
}
