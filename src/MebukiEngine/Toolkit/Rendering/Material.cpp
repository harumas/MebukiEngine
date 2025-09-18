#include "Material.h"

Material::Material() :
	handleId(-1),
	isDirty(false),
	shaderPass(nullptr),
	propertyData(0)

{
}

Material::Material(uint32_t handleId, const std::shared_ptr<ShaderPass>& shaderPass) :
	handleId(handleId),
	isDirty(true),
	shaderPass(shaderPass),
	propertyData(shaderPass->GetLayout().GetTotalSize())
{
}

void Material::SetPipelineState(const GraphicsContext& context) const
{
	if (shaderPass != nullptr)
	{
		auto pipelineState = shaderPass->GetPipelineState();
		context.SetPipelineState(pipelineState.get());
	}
	else
	{
		ThrowMessage("ShaderPass is not set for Material.");
	}
}

void Material::UploadPropertyData(GpuConstants& gpuConstants)
{
	if (!isDirty)
		return;

	gpuConstants.UploadMaterialBuffer(propertyData.data(), propertyData.size(), handleId);

	isDirty = false;
}

void Material::SetFloat(const std::string& name, float value)
{
	const auto& param = shaderPass->GetLayout().GetParameter(name);

	if (param.type == MaterialLayout::ParamType::Float)
	{
		memcpy(propertyData.data() + param.offset, &value, sizeof(float));
		isDirty = true;
		return;
	}

	throw std::runtime_error("Parameter type mismatch in Material::SetFloat");
}

void Material::SetFloat2(const std::string& name, const DirectX::XMFLOAT2& value)
{
	const auto& param = shaderPass->GetLayout().GetParameter(name);

	if (param.type == MaterialLayout::ParamType::Float2)
	{
		memcpy(propertyData.data() + param.offset, &value, sizeof(DirectX::XMFLOAT2));
		isDirty = true;
		return;
	}

	throw std::runtime_error("Parameter type mismatch in Material::SetFloat2");
}

void Material::SetFloat3(const std::string& name, const DirectX::XMFLOAT3& value)
{
	const auto& param = shaderPass->GetLayout().GetParameter(name);

	if (param.type == MaterialLayout::ParamType::Float3)
	{
		memcpy(propertyData.data() + param.offset, &value, sizeof(DirectX::XMFLOAT3));
		isDirty = true;
		return;
	}

	throw std::runtime_error("Parameter type mismatch in Material::SetFloat3");
}

void Material::SetFloat4(const std::string& name, const DirectX::XMFLOAT4& value)
{
	const auto& param = shaderPass->GetLayout().GetParameter(name);

	if (param.type == MaterialLayout::ParamType::Float4)
	{
		memcpy(propertyData.data() + param.offset, &value, sizeof(DirectX::XMFLOAT4));
		isDirty = true;
		return;
	}

	throw std::runtime_error("Parameter type mismatch in Material::SetFloat4");
}

void Material::SetInt(const std::string& name, int value)
{
	const auto& param = shaderPass->GetLayout().GetParameter(name);

	if (param.type == MaterialLayout::ParamType::Int)
	{
		memcpy(propertyData.data() + param.offset, &value, sizeof(int));
		isDirty = true;
		return;
	}

	throw std::runtime_error("Parameter type mismatch in Material::SetInt");
}

void Material::SetTexturePath(const std::wstring& path)
{
	overrideTexturePath = path;
}

const std::wstring& Material::GetTexturePath() const
{
	return overrideTexturePath;
}

uint32_t Material::GetHandleId()
{
	return handleId;
}


