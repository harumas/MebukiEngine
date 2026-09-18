#pragma once
#include <Toolkit/Component/Component.h>
#include <Toolkit/Rendering/Material.h>
#include <Toolkit/Mesh/Mesh.h>
#include <Toolkit/Math/Matrices.h>

class Renderer : public Component
{
public:
	explicit Renderer(ActorRef actorRef);

	// 同じMeshを複数のRendererで共有できるよう、コピーせずに参照を持つ
	void SetMesh(std::shared_ptr<Mesh> mesh);
	void SetMaterial(const Material& material);

	void OnPreDraw(const GraphicsContext& context, GpuConstants& gpuConstants) override;
	void OnDraw(RenderQueue& renderQueue) override;

private:
	// テクスチャを持たないRenderer全体で1枚を共有する
	// (Rendererごとに作ると512x512のリソースが描画数だけ増えてVRAMを食い潰す)
	static Texture whiteTexture;
	static UINT whiteTextureHandle;

	std::shared_ptr<Mesh> mesh;
	Material material;
	Texture texture;
	bool isResourceUpdated;
	UINT shaderResourceHandle;

	UINT transformHandle;
};
