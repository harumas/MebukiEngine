#pragma once
#include "MeshData.h";
#include <Rendering/Rendering.h>

class Mesh
{
public:
	Mesh();
	void Create(const std::string& path, D3D12_PRIMITIVE_TOPOLOGY topology);
	void Create(std::vector<Vertex> vertices, std::vector<uint16_t> indices, D3D12_PRIMITIVE_TOPOLOGY topology);

	const MeshData& GetMeshData() { return meshData; }
	D3D12_VERTEX_BUFFER_VIEW& GetVertexBufferView() { return vertexBufferView; }
	D3D12_INDEX_BUFFER_VIEW& GetIndexBufferView() { return indexBufferView; }

private:
	MeshData meshData = {};

	winrt::com_ptr<ID3D12Resource> vertexBuffer = nullptr;
	winrt::com_ptr<ID3D12Resource> indexBuffer = nullptr;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView = {};
	D3D12_INDEX_BUFFER_VIEW indexBufferView = {};

	void CreateVertexBuffer(const std::vector<Vertex>& vertices);
	void CreateIndexBuffer(const void* indices, size_t count, bool use32bit);
};
