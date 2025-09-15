#include "MaterialLayout.h"

MaterialLayout::MaterialLayout(std::initializer_list<PropertyInfo> paramList)
{
	paramMap = std::make_shared<std::unordered_map<std::string, ParamInfo>>();

	for (const auto& param : paramList)
	{
		RegisterParam(param.name, param.type);
	}
}

MaterialLayout::ParamInfo& MaterialLayout::GetParameter(const std::string& name) const
{
	if (paramMap->contains(name))
	{
		return (*paramMap)[name];
	}

	throw std::runtime_error("Parameter was not found.");
}

size_t MaterialLayout::GetTotalSize() const
{
	return currentOffset;
}

void MaterialLayout::RegisterParam(const std::string& name, ParamType type)
{
	const size_t align = 16; // CBVの基本はfloat4単位でアライン
	const size_t size = GetTypeSize(type);

	// アライメントに揃える
	if (currentOffset % align != 0)
	{
		currentOffset = (currentOffset + align - 1) & ~(align - 1);
	}

	(*paramMap)[name] = { currentOffset, size, type };
	currentOffset += align; // 次も16バイト境界から
}

size_t MaterialLayout::GetTypeSize(ParamType type)
{
	switch (type)
	{
	case ParamType::Float:  return 4;
	case ParamType::Float2: return 8;
	case ParamType::Float3: return 12;
	case ParamType::Float4: return 16;
	case ParamType::Int:    return 4;
	}
	return 0;
}

