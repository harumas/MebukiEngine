#pragma once
#include "TestWorld.h"
#include "ShaderPass/HalfLambertPass.h"
#include "Toolkit/Component/DirectionalLight.h"

void TestWorld::Initialize()
{
	auto actorService = EngineService::GetInstance().Resolve<ActorService>();

	auto dLightActor = actorService->Create(L"DirectionalLight");
	dLightActor->AddComponent<DirectionalLight>();

	//auto lightActor = actorService->Create(L"PointLight");
	//std::shared_ptr<PointLight> light = lightActor->AddComponent<PointLight>();
	//light->currentData.lightPosition = { 0.0f, 2.0f, -4.0f };
	//light->currentData.radius = 10.0f;
	//light->currentData.lightColor = Vec3{ 15.0f,15.0f,15.0f };

	auto cameraActor = actorService->Create(L"MainCamera");
	cameraActor->GetComponent<Transform>()->position = Vec3(0.0f, 0.0f, -5.0f);

	std::shared_ptr<Camera> camera = cameraActor->AddComponent<Camera>();
	Camera::current = camera;

	cube = actorService->Create(L"TestActor");

	auto shaderPassPool = EngineService::GetInstance().Resolve<ShaderPassPool>();
	auto halfLambertPass = shaderPassPool->GetShaderPass<HalfLambertPass>();
	Material material(halfLambertPass);

	Mesh mesh;
	mesh.Create("Assets/SampleModel.glb", D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	std::shared_ptr<Renderer> renderer = cube->AddComponent<Renderer>();
	renderer->SetMesh(mesh);
	renderer->SetMaterial(material);
}

void TestWorld::Update()
{
	cube->GetComponent<Transform>()->rotation.y += 0.01f;
}

