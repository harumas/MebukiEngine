#pragma once

class ShaderPass
{
public:
	void Compile(ID3D12Device* device, ID3D12RootSignature* rootSignature);
	const winrt::com_ptr<ID3D12PipelineState>& GetPipelineState() const;

protected:
	std::wstring shaderPath;
	winrt::com_ptr<ID3D12PipelineState> pipelineState;
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
};
