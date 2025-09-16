#include "ModelLoader.h"

#include "MeshData.h"
#include "StreamReader.h"


MeshData ModelLoader::Load(const std::string& path, D3D12_PRIMITIVE_TOPOLOGY topology)
{
	BinaryData data = LoadBinary(path);

	MeshData meshData;
	meshData.vertices = ConvertVertices(data);
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

	meshData.texHandle = data.texture;

	return meshData;
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

ModelLoader::BinaryData ModelLoader::LoadBinary(const std::string& path)
{
	// ストリームを作成
	const std::unique_ptr<Microsoft::glTF::GLBResourceReader> resourceReader = CreateResourceReader(path);

	// ドキュメントを読み込み(メタデータ的なやつ)
	Microsoft::glTF::Document document = LoadDocument(resourceReader);

	//単一メッシュだけ読み込む 
	auto mesh = document.meshes[0];
	auto primitive = mesh.primitives[0];

	std::string accessorId;
	BinaryData binaryData;

	// 頂点座標
	if (primitive.TryGetAttributeAccessorId(Microsoft::glTF::ACCESSOR_POSITION, accessorId))
	{
		auto accessor = document.accessors.Get(accessorId);
		binaryData.positions = resourceReader->ReadBinaryData<float>(document, accessor);
	}

	// 法線
	if (primitive.TryGetAttributeAccessorId(Microsoft::glTF::ACCESSOR_NORMAL, accessorId))
	{
		auto accessor = document.accessors.Get(accessorId);
		binaryData.normals = resourceReader->ReadBinaryData<float>(document, accessor);
	}

	// UV 
	if (primitive.TryGetAttributeAccessorId(Microsoft::glTF::ACCESSOR_TEXCOORD_0, accessorId))
	{
		auto accessor = document.accessors.Get(accessorId);
		binaryData.uvs = resourceReader->ReadBinaryData<float>(document, accessor);
	}

	// インデックス
	if (!primitive.indicesAccessorId.empty())
	{
		const auto& accessor = document.accessors.Get(primitive.indicesAccessorId);

		// インデックスは型によって分岐 (unsigned short / unsigned int)
		if (accessor.componentType == Microsoft::glTF::COMPONENT_UNSIGNED_SHORT)
		{
			binaryData.indices16 = resourceReader->ReadBinaryData<uint16_t>(document, accessor);
			binaryData.use32bitIndex = false;
		}
		else if (accessor.componentType == Microsoft::glTF::COMPONENT_UNSIGNED_INT)
		{
			binaryData.indices32 = resourceReader->ReadBinaryData<uint32_t>(document, accessor);
			binaryData.use32bitIndex = true;
		}
		else
		{
			throw std::runtime_error("Unsupported index component type");
		}
	}

	// テクスチャ読み込み (最初の画像のみ対応)
	if (!document.images.Elements().empty())
	{
		const Microsoft::glTF::Image& image = document.images.Elements().front(); // 例: 最初の画像

		// bufferViewIdから対応するBufferViewを取得
		const Microsoft::glTF::BufferView& view = document.bufferViews.Get(image.bufferViewId);

		// resources から実データにアクセス
		const std::vector<uint8_t>& bufferData = resourceReader->ReadBinaryData<uint8_t>(document, view);

		const uint8_t* dataPtr = bufferData.data();
		const size_t dataSize = view.byteLength;

		binaryData.texture = { dataPtr, dataSize };
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
