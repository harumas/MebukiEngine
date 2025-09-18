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

	MeshData Load(const std::string& path, D3D12_PRIMITIVE_TOPOLOGY topology);

private:

	struct BinaryData
	{
		std::vector<float> positions;
		std::vector<float> normals;
		std::vector<float> uvs;
		std::vector<uint16_t> indices16;
		std::vector<uint32_t> indices32;
		bool use32bitIndex = false;

		std::vector<uint8_t> textureBytes;
	};

	std::vector<Vertex> ConvertVertices(const BinaryData& binaryData);
	BinaryData LoadBinary(const std::string& path);
	Microsoft::glTF::Document LoadDocument(const std::unique_ptr<Microsoft::glTF::GLBResourceReader>& path);
	std::unique_ptr<Microsoft::glTF::GLBResourceReader> CreateResourceReader(const std::string& path);
};

