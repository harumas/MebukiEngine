#pragma once
#include "PipelineTypes.h"

struct RootParameter
{
	ParameterType type;
	UINT shaderRegister;
	UINT registerSpace;
	D3D12_ROOT_DESCRIPTOR_FLAGS descriptorFlags;
	D3D12_SHADER_VISIBILITY visibility;
	bool asTable;

	D3D12_DESCRIPTOR_RANGE_FLAGS rangeFlag = D3D12_DESCRIPTOR_RANGE_FLAG_NONE;
	D3D12_ROOT_DESCRIPTOR_FLAGS descFlag = D3D12_ROOT_DESCRIPTOR_FLAG_NONE;
};

class RootSignature
{
public:
	RootSignature();
	void Initialize(ID3D12Device* device, const std::vector<RootParameter>& rootParameters);
	ID3D12RootSignature* Get() const;

private:
	struct RootParamData
	{
		RootParamData(size_t rangesSize) : ranges(rangesSize) {}
		std::vector<CD3DX12_DESCRIPTOR_RANGE1> ranges;
		std::vector<CD3DX12_ROOT_PARAMETER1> rootParams;
	};

	winrt::com_ptr<ID3D12RootSignature> rootSignature;

	D3D12_FEATURE_DATA_ROOT_SIGNATURE GetFeatureData(ID3D12Device* device);
	RootParamData CreateRootParameters(const std::vector<RootParameter>& rootParameters);
};
