#include "ShaderPass.h"
#include <d3dcompiler.h>

#include "Toolkit/Mesh/MeshData.h"

void ShaderPass::Compile(ID3D12Device* device, ID3D12RootSignature* rootSignature)
{
	// デバッグ情報を出力する
#if defined(_DEBUG)
	UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	UINT compileFlags = 0;
#endif

	// シェーダのコンパイル
	winrt::com_ptr<ID3DBlob> vsBlob;
	winrt::com_ptr<ID3DBlob> psBlob;
	winrt::com_ptr<ID3DBlob> errorBlob;

	HRESULT result = S_OK;

	result = D3DCompileFromFile(shaderPath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "VSMain", "vs_5_0", compileFlags, 0, vsBlob.put(), errorBlob.put());
	CheckBlobHRESULT(result, errorBlob.get());

	result = D3DCompileFromFile(shaderPath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "PSMain", "ps_5_0", compileFlags, 0, psBlob.put(), errorBlob.put());
	CheckBlobHRESULT(result, errorBlob.get());

	// 頂点レイアウトの生成
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(Vertex, Position), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(Vertex, Normal),   D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, offsetof(Vertex, UV),       D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(Vertex, Tangent),  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, offsetof(Vertex, Color), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	// パイプラインステートオブジェクト(PSO)を生成
	// NOTE: RasterizerState, BlendState, DepthStencilState, etc. are configured
	// by subclass constructors before Compile() is called — do not overwrite them here.
	psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
	psoDesc.pRootSignature = rootSignature;
	psoDesc.VS = CD3DX12_SHADER_BYTECODE(vsBlob.get());
	psoDesc.PS = CD3DX12_SHADER_BYTECODE(psBlob.get());

	// パイプラインステートの作成
	winrt::check_hresult(device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS_WRT(pipelineState)));
}

const winrt::com_ptr<ID3D12PipelineState>& ShaderPass::GetPipelineState() const
{
	return pipelineState;
}

MaterialLayout& ShaderPass::GetLayout()
{
	return layout;
}
