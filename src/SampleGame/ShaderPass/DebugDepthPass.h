#pragma once
#include "Toolkit/Rendering/MaterialLayout.h"
#include "Toolkit/Rendering/ShaderPass.h"

class DebugDepthPass : public ShaderPass
{
public:
	DebugDepthPass() : ShaderPass(MaterialLayout{})
	{
		shaderPath = L"SampleGame/Shaders/DebugDepth.hlsl";

		psoDesc = {};
		psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		psoDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psoDesc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
		psoDesc.NumRenderTargets = 1;
		psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		psoDesc.SampleDesc.Count = 1;

		psoDesc.DSVFormat = DXGI_FORMAT_UNKNOWN;
		psoDesc.DepthStencilState.DepthEnable = false;
		psoDesc.DepthStencilState.StencilEnable = false;
	}
};
