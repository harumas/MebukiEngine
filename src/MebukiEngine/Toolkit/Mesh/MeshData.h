#pragma once

struct Vertex
{
	DirectX::XMFLOAT3 Position; // 位置座標
	DirectX::XMFLOAT3 Normal; // 法線
	DirectX::XMFLOAT2 UV; // uv座標
	DirectX::XMFLOAT3 Tangent; // 接空間
	DirectX::XMFLOAT4 Color; // 頂点色
};

struct MeshData
{
	std::vector<Vertex> vertices;
	std::vector<uint16_t> indices16;
	std::vector<uint32_t> indices32;
	bool use32bitIndex = false;
	D3D12_PRIMITIVE_TOPOLOGY topology = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
};
