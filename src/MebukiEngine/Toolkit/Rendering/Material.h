#pragma once
#include <Rendering/Rendering.h>	

#include "ShaderPass.h"

class Material
{
public:
	Material();
	Material(uint32_t uint32, const std::shared_ptr<ShaderPass>& shaderPass);
	void SetPipelineState(const GraphicsContext& context) const;
	void UploadPropertyData(GpuConstants& gpuConstants);

	void SetFloat(const std::string& name, float value);
	void SetFloat2(const std::string& name, const DirectX::XMFLOAT2& value);
	void SetFloat3(const std::string& name, const DirectX::XMFLOAT3& value);
	void SetFloat4(const std::string& name, const DirectX::XMFLOAT4& value);
	void SetInt(const std::string& name, int value);
	uint32_t GetHandleId();

private:
	uint32_t handleId;
	bool isDirty;

	std::shared_ptr<ShaderPass> shaderPass;
	std::vector<uint8_t> propertyData;
};

