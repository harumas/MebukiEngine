#pragma once
#include "TestWorld.h"
#include "ShaderPass/HalfLambertPass.h"
#include "ShaderPass/SkyboxPass.h"
#include "Toolkit/Component/DirectionalLight.h"
#include "Toolkit/Rendering/MaterialHandler.h"

void TestWorld::Initialize(const EngineService& engineService)
{
	auto actorService = engineService.Resolve<ActorService>();

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

	auto shaderPassPool = engineService.Resolve<ShaderPassPool>();
	auto materialHandler = engineService.Resolve<MaterialHandler>();

	//　クロワッサンの描画
	{
		cube = actorService->Create(L"TestActor");

		auto halfLambertPass = shaderPassPool->GetShaderPass<HalfLambertPass>();
		Material halfLambert = materialHandler->Create(halfLambertPass);

		Mesh croissantMesh("Assets/croissant.glb", D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		std::shared_ptr<Renderer> renderer = cube->AddComponent<Renderer>();
		renderer->SetMesh(croissantMesh);
		renderer->SetMaterial(halfLambert);

		auto transform = cube->GetComponent<Transform>();

		transform->rotation.z = XM_PIDIV2;
		transform->rotation.y = XM_PI * 0.85f;
		transform->rotation.x = -XM_PIDIV2;

		transform->scale = Vec3(0.5f, 0.5f, 0.5f);

		transform->position.y = -0.5f;

	}

	//　球体の描画
	{
		//acube = actorService->Create(L"Cube");

		//auto halfLambertPass = shaderPassPool->GetShaderPass<HalfLambertPass>();
		//Material halfLambert = materialHandler->Create(halfLambertPass);

		//std::shared_ptr<Renderer> renderer = acube->AddComponent<Renderer>();
		//renderer->SetMesh(PrimitiveMesh::CreateCube());
		//renderer->SetMaterial(halfLambert);

		//acube->GetComponent<Transform>()->position.x += 1;
	}

	// スカイボックスの描画
	{
		auto skyboxPass = shaderPassPool->GetShaderPass<SkyboxPass>();
		Material skyBox = materialHandler->Create(skyboxPass);
		skyBox.SetTexturePath(L"Assets/sky_14_2k.png");

		Mesh skyboxMesh("Assets/skybox.glb", D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		auto skyboxActor = actorService->Create(L"Skybox");
		skyboxActor->GetComponent<Transform>()->scale = Vec3(300.0f, 300.0f, 300.0f);

		std::shared_ptr<Renderer> skyboxRenderer = skyboxActor->AddComponent<Renderer>();
		skyboxRenderer->SetMesh(skyboxMesh);
		skyboxRenderer->SetMaterial(skyBox);
	}
}

void TestWorld::Update()
{
}

