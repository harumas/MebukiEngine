#include "RootSignature.h"

RootSignature::RootSignature()
{
}

void RootSignature::Initialize(ID3D12Device* device, const std::vector<RootParameter>& rootParameters)
{
	D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = GetFeatureData(device);

	// RootParameterの作成
	std::vector<CD3DX12_ROOT_PARAMETER1> dxRootParameters = CreateRootParameters(rootParameters);

	// サンプラーの生成
	// テクスチャデータからどう色を取り出すかを決めるための設定
	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);

	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
	rootSignatureDesc.Init_1_1(dxRootParameters.size(), dxRootParameters.data(), 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	winrt::com_ptr<ID3DBlob> errorBlob;
	winrt::com_ptr<ID3DBlob> serializedRootSignature;
	HRESULT result = D3DX12SerializeVersionedRootSignature(
		&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1,
		serializedRootSignature.put(),
		errorBlob.put());
	CheckBlobHRESULT(result, errorBlob.get());

	winrt::check_hresult(device->CreateRootSignature(0, serializedRootSignature->GetBufferPointer(), serializedRootSignature->GetBufferSize(), IID_PPV_ARGS_WRT(rootSignature)));
}

ID3D12RootSignature* RootSignature::Get() const
{
	return rootSignature.get();
}

D3D12_FEATURE_DATA_ROOT_SIGNATURE RootSignature::GetFeatureData(ID3D12Device* device)
{
	D3D12_FEATURE_DATA_ROOT_SIGNATURE featureVersion = {};

	// サポートするRootSignatureのバージョンを取得
	featureVersion.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

	if (FAILED(device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureVersion, sizeof(featureVersion))))
	{
		featureVersion.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
	}

	return featureVersion;
}

std::vector<CD3DX12_ROOT_PARAMETER1> RootSignature::CreateRootParameters(const std::vector<RootParameter>& rootParameters)
{
	std::vector<CD3DX12_DESCRIPTOR_RANGE1> ranges(rootParameters.size());
	std::vector<CD3DX12_ROOT_PARAMETER1> rootParams;

	for (size_t i = 0; i < rootParameters.size(); ++i)
	{
		const RootParameter& rootParameter = rootParameters[i];
		CD3DX12_DESCRIPTOR_RANGE1& range = ranges[i];
		CD3DX12_ROOT_PARAMETER1 rootParam;

		switch (rootParameter.type)
		{
		case ParameterType::CBV:
			if (rootParameter.asTable)
			{
				range.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, rootParameter.shaderRegister, rootParameter.registerSpace, rootParameter.rangeFlag);
				rootParam.InitAsDescriptorTable(1, &range, rootParameter.visibility);
			}
			else
			{
				rootParam.InitAsConstantBufferView(rootParameter.shaderRegister, rootParameter.registerSpace, rootParameter.descriptorFlags, rootParameter.visibility);
			}
			break;

		case ParameterType::SRV:
			if (rootParameter.asTable)
			{
				range.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, rootParameter.shaderRegister, rootParameter.registerSpace, rootParameter.rangeFlag);
				rootParam.InitAsDescriptorTable(1, &range, rootParameter.visibility);
			}
			else
			{
				rootParam.InitAsShaderResourceView(rootParameter.shaderRegister, rootParameter.registerSpace, rootParameter.descriptorFlags, rootParameter.visibility);
			}
			break;

		case ParameterType::UAV:
			if (rootParameter.asTable)
			{
				range.Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, rootParameter.shaderRegister, rootParameter.registerSpace, rootParameter.rangeFlag);
				rootParam.InitAsDescriptorTable(1, &range, rootParameter.visibility);
			}
			else
			{
				rootParam.InitAsUnorderedAccessView(rootParameter.shaderRegister, rootParameter.registerSpace, rootParameter.descriptorFlags, rootParameter.visibility);
			}
			break;

		default:
			break;
		}

		rootParams.push_back(rootParam);
	}

	return rootParams;
}
