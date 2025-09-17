#include "Renderer.h"
#include "Camera.h"

void Renderer::SetMesh(const Mesh& mesh)
{
	this->mesh = mesh;
}

void Renderer::SetMaterial(const Material& material)
{
	this->material = material;
}

void Renderer::OnPreDraw(const GraphicsContext& context, GpuConstants& gpuConstants)
{
	// モデル行列を取得 
	const DirectX::XMMATRIX modelMatrix = gameObject.lock()->GetComponent<Transform>()->GetMatrix();

	// HLSL側で受け取れるレイアウトに変換
	// XMMATRIXは行優先、HLSLは列優先のため、転置してから変換する
	XMFLOAT4X4 model4X4;
	XMStoreFloat4x4(&model4X4, XMMatrixTranspose(modelMatrix));

	// Transform用の定数バッファに登録し、オフセットを取得
	transformHandle = gpuConstants.AddTransformData(model4X4);

	// テクスチャデータをGPUに転送 
	if (!isResourceUpdated && mesh.HasTexture())
	{
		Texture texture = mesh.GetMeshData().textureData;
		texture.UpdateSubResources(context);
		shaderResourceHandle = gpuConstants.CreateShaderResourceView(texture.GetTextureResource(), DXGI_FORMAT_R8G8B8A8_UNORM);
		isResourceUpdated = true;
	}
}

void Renderer::OnDraw(const GraphicsContext& context, const GpuConstants& gpuConstants)
{
	// 現在のTransformに定数バッファのオフセットを設定
	gpuConstants.SetTransformCBV(context, transformHandle);

	// 現在のMaterialに定数バッファのオフセットを設定
	gpuConstants.SetMaterialCBV(context, material.GetHandleId());

	// ShaderResourceViewのハンドルをセット 
	gpuConstants.SetGraphicsRootDescriptorTable(context, shaderResourceHandle);

	// PipelineStateを設定
	material.SetPipelineState(context);

	// トポロジー情報、頂点バッファ、インデックスバッファをIAステージに設定
	const MeshData& meshData = mesh.GetMeshData();
	context.SetPrimitiveTopology(meshData.topology);
	context.SetVertexBuffer(0, mesh.GetVertexBufferView());
	context.SetIndexBuffer(mesh.GetIndexBufferView());

	int indicesCount = meshData.use32bitIndex ? meshData.indices32.size() : meshData.indices16.size();

	// 描画コマンドを発行
	context.Draw(indicesCount, 0);
}
