#pragma once
#include "MaterialLayout.h"

class ShaderPass
{
public:
	void Compile(ID3D12Device* device, ID3D12RootSignature* rootSignature);
	const winrt::com_ptr<ID3D12PipelineState>& GetPipelineState() const;

	MaterialLayout& GetLayout();

protected:
	explicit ShaderPass(const MaterialLayout& layout)
		: layout(layout)
	{
	}

	std::wstring shaderPath;
	winrt::com_ptr<ID3D12PipelineState> pipelineState;
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	MaterialLayout layout;
};
