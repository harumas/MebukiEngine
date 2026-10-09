#pragma once
#include "Toolkit/Rendering/Texture.h"

struct Vertex
{
	DirectX::XMFLOAT3 Position; // 位置座標
	DirectX::XMFLOAT3 Normal; // 法線
	DirectX::XMFLOAT2 UV; // uv座標
	DirectX::XMFLOAT3 Tangent; // 接空間
	DirectX::XMFLOAT4 Color; // 頂点色
};

// glTFの画像1枚分。同じ画像を参照する全プリミティブで共有し、GPUへの転送は最初の1回だけ行う
struct ModelTexture
{
	std::vector<uint8_t> imageBytes;
	Texture texture;
	UINT shaderResourceHandle = static_cast<UINT>(-1);
};

struct MeshData
{
	std::vector<Vertex> vertices;
	std::vector<uint16_t> indices16;
	std::vector<uint32_t> indices32;
	bool use32bitIndex = false;

	D3D12_PRIMITIVE_TOPOLOGY topology = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	// glTFのノード階層から算出した、ファイル内での配置を表すワールド変換
	DirectX::XMFLOAT4X4 nodeTransform =
	{
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};

	// マテリアルのBaseColorテクスチャ (無ければnullptr)
	std::shared_ptr<ModelTexture> baseColorTexture;
	Texture textureData;
};
