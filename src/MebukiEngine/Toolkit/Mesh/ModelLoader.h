#pragma once
#include <GLTFSDK/GLTF.h>
#include <GLTFSDK/GLBResourceReader.h>
#include <GLTFSDK/Deserialize.h>

#include "MeshData.h"

class ModelLoader
{
public:
	ModelLoader() = default;
	~ModelLoader() = default;

	// glTFに含まれる全メッシュ・全プリミティブを、プリミティブ単位のMeshDataとして返す
	std::vector<MeshData> Load(const std::string& path, D3D12_PRIMITIVE_TOPOLOGY topology);

private:

	struct BinaryData
	{
		std::vector<float> positions;
		std::vector<float> normals;
		std::vector<float> uvs;
		std::vector<uint16_t> indices16;
		std::vector<uint32_t> indices32;
		bool use32bitIndex = false;
	};

	std::vector<Vertex> ConvertVertices(const BinaryData& binaryData);

	// プリミティブ1つ分の頂点・インデックスを読む
	BinaryData ReadPrimitive(Microsoft::glTF::Document& document,
		Microsoft::glTF::GLBResourceReader& resourceReader,
		const Microsoft::glTF::MeshPrimitive& primitive);

	// ノードを再帰的に辿り、親からの変換を累積しながらメッシュを集める
	void CollectNode(Microsoft::glTF::Document& document,
		Microsoft::glTF::GLBResourceReader& resourceReader,
		const std::string& nodeId,
		const DirectX::XMMATRIX& parentMatrix,
		D3D12_PRIMITIVE_TOPOLOGY topology,
		std::vector<MeshData>& meshDataList);

	// プリミティブのマテリアルが参照するBaseColor画像を取り出す (無ければnullptr)
	std::shared_ptr<ModelTexture> GetBaseColorTexture(Microsoft::glTF::Document& document,
		Microsoft::glTF::GLBResourceReader& resourceReader,
		const Microsoft::glTF::MeshPrimitive& primitive);

	// ノード単体のローカル変換を取り出す
	static DirectX::XMMATRIX GetNodeLocalMatrix(const Microsoft::glTF::Node& node);
	Microsoft::glTF::Document LoadDocument(const std::unique_ptr<Microsoft::glTF::GLBResourceReader>& path);
	std::unique_ptr<Microsoft::glTF::GLBResourceReader> CreateResourceReader(const std::string& path);

	// 読み込み中のファイル内で、画像ID → 共有テクスチャ
	std::unordered_map<std::string, std::shared_ptr<ModelTexture>> textureCache;
};

