#pragma once
#include <Toolkit/Component/Component.h>
#include <Toolkit/Rendering/Material.h>
#include <Toolkit/Mesh/Mesh.h>
#include <Toolkit/Math/Matrices.h>

class Renderer : public Component
{
public:
	explicit Renderer(const std::shared_ptr<Actor>& actorRef) : Component(actorRef), material(nullptr)
	{
	}

	void SetMesh(const Mesh& mesh)
	{
		this->mesh = mesh;
	}

	void SetMaterial(const Material& material)
	{
		this->material = material;
	}

	void OnDraw(const GraphicsContext& context) override;

private:
	Mesh mesh;
	Material material;

	ConstantBuffer<Matrices> matrixBuffer;
};
