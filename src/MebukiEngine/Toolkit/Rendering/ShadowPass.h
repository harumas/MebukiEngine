#pragma once
#include "MaterialLayout.h"
#include "ShaderPass.h"

class ShadowPass : public ShaderPass
{
public:
	ShadowPass() : ShaderPass(MaterialLayout{})
	{
		shaderPath = L"MebukiEngine/Shaders/Shadow.hlsl";

		psoDesc = {};
		psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		psoDesc.RasterizerState.DepthBias = 10000;
		psoDesc.RasterizerState.SlopeScaledDepthBias = 1.5f;

		psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		psoDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psoDesc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
		psoDesc.NumRenderTargets = 0;
		psoDesc.SampleDesc.Count = 1;

		psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
		psoDesc.DepthStencilState.DepthEnable = true;
		psoDesc.DepthStencilState.StencilEnable = false;
		psoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	}
};
