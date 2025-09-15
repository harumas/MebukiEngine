#pragma once

class MaterialLayout
{

public:
	enum class ParamType
	{
		Float,
		Float2,
		Float3,
		Float4,
		Int,
	};

	struct PropertyInfo
	{
		std::string name;
		ParamType type;
	};

	struct ParamInfo
	{
		size_t offset;   // バイトオフセット
		size_t size;     // 実際のサイズ
		ParamType type;
	};

	explicit MaterialLayout(std::initializer_list<PropertyInfo> paramList);

	ParamInfo& GetParameter(const std::string& name) const;

	size_t GetTotalSize() const;

private:
	std::shared_ptr<std::unordered_map<std::string, ParamInfo>>	 paramMap;
	size_t currentOffset = 0;

	void RegisterParam(const std::string& name, ParamType type);
	size_t GetTypeSize(ParamType type);
};
