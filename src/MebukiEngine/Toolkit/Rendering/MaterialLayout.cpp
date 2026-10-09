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
	constexpr size_t registerSize = 16; // HLSLの1レジスタ(float4)のバイト数
	const size_t size = GetTypeSize(type);

	// 今のオフセットに置くとレジスタ境界をまたぐ場合のみ、次のレジスタ先頭まで進める
	// (HLSLのcbufferパッキングと同じルール: またがない限り同じレジスタに詰め込む)
	const size_t offsetInRegister = currentOffset % registerSize;
	if (offsetInRegister + size > registerSize)
	{
		currentOffset += registerSize - offsetInRegister;
	}

	(*paramMap)[name] = { currentOffset, size, type };
	currentOffset += size;
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

