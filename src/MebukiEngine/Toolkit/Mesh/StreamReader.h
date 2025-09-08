#pragma once
#include <filesystem>

class StreamReader : public Microsoft::glTF::IStreamReader
{
public:
	// Resolves the relative URIs of any external resources declared in the glTF manifest
	std::shared_ptr<std::istream> GetInputStream(const std::string& filePath) const override
	{
		auto path = std::filesystem::path(filePath);
		auto stream = std::make_shared<std::ifstream>(path, std::ios_base::binary);

		// Check if the stream has no errors and is ready for I/O operations
		if (!stream || !(*stream))
		{
			throw std::runtime_error("Unable to create a valid input stream for uri: " + filePath);
		}

		return stream;
	}
};
