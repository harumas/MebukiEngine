#include "Renderer.h"
#include "Camera.h"

void Renderer::OnDraw(const GraphicsContext& context)
{
	material.SetPass(context);

	const MeshData& meshData = mesh.GetMeshData();
	context.SetPrimitiveTopology(meshData.topology);
	context.SetVertexBuffer(0, mesh.GetVertexBufferView());
	context.SetIndexBuffer(mesh.GetIndexBufferView());

	const DirectX::XMMATRIX world = gameObject.lock()->GetComponent<Transform>()->GetMatrix();
	const DirectX::XMMATRIX viewProj = Camera::current->GetViewMatrix() * Camera::current->GetProjectionMatrix(context.GetAspectRatio());

	matrixBuffer.SetBufferData({ world,viewProj });
	matrixBuffer.SetConstantBufferView(context, 0);

	context.Draw(meshData.indices.size(), 0);
}
