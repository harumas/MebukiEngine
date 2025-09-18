#pragma once
#include <Toolkit/Component/Component.h>
#include <Toolkit/Rendering/Material.h>
#include <Toolkit/Mesh/Mesh.h>
#include <Toolkit/Math/Matrices.h>

class Renderer : public Component
{
public:
	explicit Renderer(const std::shared_ptr<Actor>& actorRef);

	void SetMesh(const Mesh& mesh);
	void SetMaterial(const Material& material);

	void OnPreDraw(const GraphicsContext& context, GpuConstants& gpuConstants) override;
	void OnDraw(const GraphicsContext& context, const GpuConstants& gpuConstants) override;

private:
	Mesh mesh;
	Material material;
	Texture texture;
	bool isResourceUpdated;
	UINT shaderResourceHandle;

	UINT transformHandle;
};
