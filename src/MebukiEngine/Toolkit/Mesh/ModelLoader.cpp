#include "ModelLoader.h"

#include "MeshData.h"
#include "StreamReader.h"
#include "Basic/Profiler.h"


std::vector<MeshData> ModelLoader::Load(const std::string& path, D3D12_PRIMITIVE_TOPOLOGY topology)
{
	std::unique_ptr<Microsoft::glTF::GLBResourceReader> resourceReader;
	Microsoft::glTF::Document document;
	{
		ScopedTimer timer("  model: open glb + parse JSON");
		resourceReader = CreateResourceReader(path);
		document = LoadDocument(resourceReader);
	}

	std::vector<MeshData> meshDataList;
	textureCache.clear();

	// シーンのルートノードから再帰的に辿る (メッシュはノードから参照されて初めて配置が決まる)
	const Microsoft::glTF::Scene& scene = document.GetDefaultScene();

	for (const std::string& rootNodeId : scene.nodes)
	{
		CollectNode(document, *resourceReader, rootNodeId, DirectX::XMMatrixIdentity(), topology, meshDataList);
	}

	return meshDataList;
}

std::vector<Vertex> ModelLoader::ConvertVertices(const BinaryData& binaryData)
{
	size_t vertexCount = binaryData.positions.size() / 3;
	std::vector<Vertex> vertices(vertexCount);

	for (size_t i = 0; i < vertexCount; ++i)
	{
		Vertex& vertex = vertices[i];
		// 頂点座標
		vertex.Position.x = binaryData.positions[i * 3 + 0];
		vertex.Position.y = binaryData.positions[i * 3 + 1];
		vertex.Position.z = binaryData.positions[i * 3 + 2];

		// 法線
		if (!binaryData.normals.empty())
		{
			vertex.Normal.x = binaryData.normals[i * 3 + 0];
			vertex.Normal.y = binaryData.normals[i * 3 + 1];
			vertex.Normal.z = binaryData.normals[i * 3 + 2];
		}
		else
		{
			vertex.Normal = { 0,0,0 };
		}

		// UV
		if (!binaryData.uvs.empty())
		{
			vertex.UV.x = binaryData.uvs[i * 2 + 0];
			vertex.UV.y = binaryData.uvs[i * 2 + 1];
		}
		else
		{
			vertex.UV = { 0,0 };
		}

		// 接空間
		vertex.Tangent = { 0,0,0 };
		// 頂点色
		vertex.Color = { 1,1,1,1 };
	}

	return vertices;
}

DirectX::XMMATRIX ModelLoader::GetNodeLocalMatrix(const Microsoft::glTF::Node& node)
{
	switch (node.GetTransformationType())
	{
	case Microsoft::glTF::TRANSFORMATION_MATRIX:
	{
		// glTFの行列は列優先。行優先のXMFLOAT4X4にそのまま詰めると転置になり、
		// このエンジンが使っている行ベクトル規約(Transform::GetMatrixと同じ)の行列になる
		const std::array<float, 16>& v = node.matrix.values;
		const XMFLOAT4X4 m(
			v[0], v[1], v[2], v[3],
			v[4], v[5], v[6], v[7],
			v[8], v[9], v[10], v[11],
			v[12], v[13], v[14], v[15]);

		return XMLoadFloat4x4(&m);
	}

	case Microsoft::glTF::TRANSFORMATION_TRS:
	{
		const XMVECTOR scale = XMVectorSet(node.scale.x, node.scale.y, node.scale.z, 0.0f);
		const XMVECTOR rotation = XMVectorSet(node.rotation.x, node.rotation.y, node.rotation.z, node.rotation.w);
		const XMVECTOR translation = XMVectorSet(node.translation.x, node.translation.y, node.translation.z, 0.0f);

		return XMMatrixScalingFromVector(scale)
			* XMMatrixRotationQuaternion(rotation)
			* XMMatrixTranslationFromVector(translation);
	}

	default:
		return XMMatrixIdentity();
	}
}

void ModelLoader::CollectNode(
	Microsoft::glTF::Document& document,
	Microsoft::glTF::GLBResourceReader& resourceReader,
	const std::string& nodeId,
	const DirectX::XMMATRIX& parentMatrix,
	D3D12_PRIMITIVE_TOPOLOGY topology,
	std::vector<MeshData>& meshDataList)
{
	const Microsoft::glTF::Node& node = document.nodes.Get(nodeId);

	// 行ベクトル規約なので「ローカル→親」の順で掛ける
	const XMMATRIX worldMatrix = GetNodeLocalMatrix(node) * parentMatrix;

	if (!node.meshId.empty())
	{
		const Microsoft::glTF::Mesh& mesh = document.meshes.Get(node.meshId);

		for (const auto& primitive : mesh.primitives)
		{
			BinaryData data;
			{
				ScopedTimer timer("  model: read accessors");
				data = ReadPrimitive(document, resourceReader, primitive);
			}

			MeshData meshData;
			{
				ScopedTimer timer("  model: convert vertices");
				meshData.vertices = ConvertVertices(data);
			}
			meshData.topology = topology;
			meshData.use32bitIndex = data.use32bitIndex;

			if (data.use32bitIndex)
			{
				meshData.indices32 = std::move(data.indices32);
			}
			else
			{
				meshData.indices16 = std::move(data.indices16);
			}

			XMStoreFloat4x4(&meshData.nodeTransform, worldMatrix);
			{
				ScopedTimer timer("  model: read texture bytes");
				meshData.baseColorTexture = GetBaseColorTexture(document, resourceReader, primitive);
			}

			meshDataList.push_back(std::move(meshData));
		}
	}

	for (const std::string& childId : node.children)
	{
		CollectNode(document, resourceReader, childId, worldMatrix, topology, meshDataList);
	}
}

std::shared_ptr<ModelTexture> ModelLoader::GetBaseColorTexture(
	Microsoft::glTF::Document& document,
	Microsoft::glTF::GLBResourceReader& resourceReader,
	const Microsoft::glTF::MeshPrimitive& primitive)
{
	if (primitive.materialId.empty())
	{
		return nullptr;
	}

	// プリミティブ → マテリアル → テクスチャ → 画像 の順に辿る
	const Microsoft::glTF::Material& material = document.materials.Get(primitive.materialId);
	const std::string& textureId = material.metallicRoughness.baseColorTexture.textureId;

	if (textureId.empty())
	{
		return nullptr;
	}

	const std::string& imageId = document.textures.Get(textureId).imageId;

	// 同じ画像は1つのModelTextureを共有する (プリミティブごとに作るとVRAMが描画数分必要になる)
	if (const auto it = textureCache.find(imageId); it != textureCache.end())
	{
		return it->second;
	}

	const Microsoft::glTF::Image& image = document.images.Get(imageId);
	const Microsoft::glTF::BufferView& view = document.bufferViews.Get(image.bufferViewId);

	auto modelTexture = std::make_shared<ModelTexture>();
	modelTexture->imageBytes = resourceReader.ReadBinaryData<uint8_t>(document, view);

	textureCache.emplace(imageId, modelTexture);
	return modelTexture;
}

ModelLoader::BinaryData ModelLoader::ReadPrimitive(
	Microsoft::glTF::Document& document,
	Microsoft::glTF::GLBResourceReader& resourceReader,
	const Microsoft::glTF::MeshPrimitive& primitive)
{
	std::string accessorId;
	BinaryData binaryData;

	// 頂点座標
	if (primitive.TryGetAttributeAccessorId(Microsoft::glTF::ACCESSOR_POSITION, accessorId))
	{
		auto accessor = document.accessors.Get(accessorId);
		binaryData.positions = resourceReader.ReadBinaryData<float>(document, accessor);
	}

	// 法線
	if (primitive.TryGetAttributeAccessorId(Microsoft::glTF::ACCESSOR_NORMAL, accessorId))
	{
		auto accessor = document.accessors.Get(accessorId);
		binaryData.normals = resourceReader.ReadBinaryData<float>(document, accessor);
	}

	// UV
	if (primitive.TryGetAttributeAccessorId(Microsoft::glTF::ACCESSOR_TEXCOORD_0, accessorId))
	{
		auto accessor = document.accessors.Get(accessorId);
		binaryData.uvs = resourceReader.ReadBinaryData<float>(document, accessor);
	}

	// インデックス
	if (!primitive.indicesAccessorId.empty())
	{
		const auto& accessor = document.accessors.Get(primitive.indicesAccessorId);

		// インデックスは型によって分岐 (unsigned short / unsigned int)
		if (accessor.componentType == Microsoft::glTF::COMPONENT_UNSIGNED_SHORT)
		{
			binaryData.indices16 = resourceReader.ReadBinaryData<uint16_t>(document, accessor);
			binaryData.use32bitIndex = false;
		}
		else if (accessor.componentType == Microsoft::glTF::COMPONENT_UNSIGNED_INT)
		{
			binaryData.indices32 = resourceReader.ReadBinaryData<uint32_t>(document, accessor);
			binaryData.use32bitIndex = true;
		}
		else
		{
			throw std::runtime_error("Unsupported index component type");
		}
	}

	return binaryData;
}

Microsoft::glTF::Document ModelLoader::LoadDocument(const std::unique_ptr<Microsoft::glTF::GLBResourceReader>& resourceReader)
{
	//glTFドキュメントの読み込み
	std::string manifest = resourceReader->GetJson();

	Microsoft::glTF::Document document;

	try
	{
		// デシリアライズ
		document = Microsoft::glTF::Deserialize(manifest);
	}
	catch (const Microsoft::glTF::GLTFException& ex)
	{
		std::stringstream ss;

		ss << "Microsoft::glTF::Deserialize failed: ";
		ss << ex.what();

		throw std::runtime_error(ss.str());
	}

	return document;
}

std::unique_ptr<Microsoft::glTF::GLBResourceReader> ModelLoader::CreateResourceReader(const std::string& path)
{
	static const std::filesystem::path GLB_EXTENSION = L".glb";

	std::filesystem::path fsPath(path);

	//フィアル名が存在するか 
	if (!fsPath.has_filename())
	{
		throw std::runtime_error("Command line argument path has no filename");
	}

	//拡張子が存在するか
	if (!fsPath.has_extension())
	{
		throw std::runtime_error("Command line argument path has no filename extension");
	}

	//拡張子が.glbであるか
	if (fsPath.extension() != GLB_EXTENSION)
	{
		throw std::runtime_error("Only .glb files are supported in this Loader");
	}

	//ストリームの作成 
	auto streamReader = std::make_unique<StreamReader>();
	auto glbStream = streamReader->GetInputStream(path);
	auto glbResourceReader = std::make_unique<Microsoft::glTF::GLBResourceReader>(std::move(streamReader), std::move(glbStream));

	if (!glbResourceReader)
	{
		throw std::runtime_error("Command line argument path filename extension must be .gltf or .glb");
	}

	return glbResourceReader;
}
