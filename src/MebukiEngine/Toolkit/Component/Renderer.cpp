#include "Renderer.h"
#include "Camera.h"
#include <Toolkit/Actor/Actor.h>
#include <Rendering/GraphicsContext.h>
#include <Rendering/GpuConstants.h>
#include <Rendering/RenderQueue.h>

Texture Renderer::whiteTexture;
UINT Renderer::whiteTextureHandle = static_cast<UINT>(-1);

Renderer::Renderer(ActorRef actorRef) :
	Component(actorRef),
	isResourceUpdated(false),
	shaderResourceHandle(-1),
	transformHandle(-1)
{}

void Renderer::SetMesh(std::shared_ptr<Mesh> mesh)
{
	this->mesh = std::move(mesh);
}

void Renderer::SetMaterial(const Material& material)
{
	this->material = material;
}

void Renderer::OnPreDraw(const GraphicsContext& context, GpuConstants& gpuConstants)
{
	// モデル行列を取得 (glTFのノード変換 → Actorの変換 の順で適用する)
	const DirectX::XMMATRIX nodeMatrix = XMLoadFloat4x4(&mesh->GetMeshData().nodeTransform);
	const DirectX::XMMATRIX modelMatrix = nodeMatrix * actor->GetComponent<Transform>()->GetMatrix();

	// HLSL側で受け取れるレイアウトに変換
	XMFLOAT4X4 model4X4;
	XMStoreFloat4x4(&model4X4, modelMatrix);

	// プロパティ情報を転送する
	material.UploadPropertyData(gpuConstants);

	// Transform用の定数バッファに登録し、オフセットを取得
	transformHandle = gpuConstants.AddTransformData(model4X4);

	// テクスチャデータをGPUに転送 
	if (!isResourceUpdated)
	{
		if (!material.GetTexturePath().empty())
		{
			texture.Create(context, material.GetTexturePath());
			shaderResourceHandle = gpuConstants.CreateShaderResourceView(texture.GetTextureResource(), DXGI_FORMAT_R8G8B8A8_UNORM);
		}
		else if (mesh->HasTexture())
		{
			// 同じ画像を使う他のRendererが転送済みなら、そのSRVを使い回す
			ModelTexture& modelTexture = *mesh->GetMeshData().baseColorTexture;

			if (modelTexture.shaderResourceHandle == static_cast<UINT>(-1))
			{
				modelTexture.texture.Create(context, modelTexture.imageBytes);
				modelTexture.shaderResourceHandle = gpuConstants.CreateShaderResourceView(modelTexture.texture.GetTextureResource(), DXGI_FORMAT_R8G8B8A8_UNORM);

				// デコードしてGPUへ転送したので、PNGのバイト列はもう要らない
				std::vector<uint8_t>().swap(modelTexture.imageBytes);
			}

			shaderResourceHandle = modelTexture.shaderResourceHandle;
		}
		else
		{
			if (whiteTextureHandle == static_cast<UINT>(-1))
			{
				whiteTexture.Create(context, L"SampleGame/Assets/WhiteTexture.png");
				whiteTextureHandle = gpuConstants.CreateShaderResourceView(whiteTexture.GetTextureResource(), DXGI_FORMAT_R8G8B8A8_UNORM);
			}

			shaderResourceHandle = whiteTextureHandle;
		}

		if (!mesh->IsUploaded())  // Step 2-1のフラグを見るgetter
		{
			mesh->RecordUpload(context.GetCommandList());
		}

		isResourceUpdated = true;

		// UploadBufferのリリースチェック 
		mesh->TickUploadBufferRelease();
	}
}

void Renderer::OnDraw(RenderQueue& renderQueue)
{
	// この時点ではGPUコマンドは発行せず、描画に必要な情報をキューに積むだけ
	renderQueue.Submit({ mesh.get(), &material, transformHandle, shaderResourceHandle });
}
