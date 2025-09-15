#include "Renderer.h"
#include "Camera.h"

void Renderer::OnPreDraw(const GraphicsContext& context, GpuConstants& gpuConstants)
{
	const DirectX::XMMATRIX modelMatrix = gameObject.lock()->GetComponent<Transform>()->GetMatrix();

	XMFLOAT4X4 model4X4;
	XMStoreFloat4x4(&model4X4, XMMatrixTranspose(modelMatrix));

	drawHandle = gpuConstants.AddTransformData(model4X4);
}

void Renderer::OnDraw(const GraphicsContext& context, const GpuConstants& gpuConstants)
{
	gpuConstants.SetTransformCBV(context, drawHandle);
	gpuConstants.SetMaterialCBV(context, material.GetHandleId());

	material.SetPass(context);

	const MeshData& meshData = mesh.GetMeshData();
	context.SetPrimitiveTopology(meshData.topology);
	context.SetVertexBuffer(0, mesh.GetVertexBufferView());
	context.SetIndexBuffer(mesh.GetIndexBufferView());

	int indicesCount = meshData.use32bitIndex ? meshData.indices32.size() : meshData.indices16.size();

	context.Draw(indicesCount, 0);
}
