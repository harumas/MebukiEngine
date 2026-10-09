#include "TestWorld.h"
#include "ShaderPass/HalfLambertPass.h"
#include "ShaderPass/SkyboxPass.h"
#include "Component/DepthDebugComponent.h"
#include "Component/InputDebugComponent.h"
#include "Component/FirstPersonCamera.h"
#include "Toolkit/Component/DirectionalLight.h"
#include "Toolkit/Rendering/MaterialHandler.h"
#include "Rendering/GraphicsDevice.h"
#include "Basic/Profiler.h"

void TestWorld::Initialize(const EngineService& engineService)
{
	auto actorService = engineService.Resolve<ActorService>();
	ID3D12Device* device = engineService.Resolve<GraphicsDevice>()->Get();

	auto dLightActor = actorService->Create(L"DirectionalLight");
	dLightActor->AddComponent<DirectionalLight>();

	auto lightActor = actorService->Create(L"PointLight");
	std::shared_ptr<PointLight> light = lightActor->AddComponent<PointLight>();
	light->currentData.lightPosition = { 0.0f, 2.0f, -4.0f };
	light->currentData.radius = 10.0f;
	light->currentData.lightColor = Vec3{ 15.0f,15.0f,15.0f };

	auto cameraActor = actorService->Create(L"MainCamera");
	// Sponzaは約36m(X) x 20m(Y) x 24m(Z)。回廊の端に立って長辺(+X方向)を見渡す位置
	auto cameraTransform = cameraActor->GetComponent<Transform>();
	cameraTransform->position = Vec3(-14.0f, 2.0f, 2.5f);
	cameraTransform->rotation.y = XM_PIDIV2;

	// WASDで移動、右ドラッグで視点操作 (Q/Eで上下、Shiftで加速)
	std::shared_ptr<FirstPersonCamera> camera = cameraActor->AddComponent<FirstPersonCamera>();
	camera->Setup(engineService);

	auto shaderPassPool = engineService.Resolve<ShaderPassPool>();
	auto materialHandler = engineService.Resolve<MaterialHandler>();

	//　クロワッサンの描画
	{
		auto cube = actorService->Create(L"TestActor");

		auto halfLambertPass = shaderPassPool->GetShaderPass<HalfLambertPass>();
		Material halfLambert = materialHandler->Create(halfLambertPass);
		halfLambert.SetFloat4("BaseColor", DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		auto croissantMesh = std::make_shared<Mesh>(device, "SampleGame/Assets/croissant.glb", D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		std::shared_ptr<Renderer> renderer = cube->AddComponent<Renderer>();
		renderer->SetMesh(croissantMesh);
		renderer->SetMaterial(halfLambert);

		auto transform = cube->GetComponent<Transform>();

		transform->rotation.z = XM_PIDIV2;
		transform->rotation.y = XM_PI * 0.85f;
		transform->rotation.x = -XM_PIDIV2;

		transform->scale = Vec3(0.5f, 0.5f, 0.5f);

		// Sponzaの床(y≒0)の上に置く
		transform->position = Vec3(-10.0f, 0.6f, 2.5f);
	}

	// Sponzaの読み込み (本体 + カーテンのアドオン)
	// 本体と同じ座標系なので、どちらもそのまま置けば噛み合う
	{
		auto halfLambertPass = shaderPassPool->GetShaderPass<HalfLambertPass>();
		Material sponzaMaterial = materialHandler->Create(halfLambertPass);
		sponzaMaterial.SetFloat4("BaseColor", DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		const char* sponzaModels[] =
		{
			"SampleGame/Assets/NewSponza_Main.glb",
			"SampleGame/Assets/NewSponza_Curtains.glb",
		};

		for (const char* modelPath : sponzaModels)
		{
			std::vector<std::shared_ptr<Mesh>> partMeshes;
			{
				ScopedTimer timer("  Mesh::LoadAll");
				partMeshes = Mesh::LoadAll(device, modelPath, D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			}

			ScopedTimer timer("  spawn actors");

			// 1プリミティブ = 1Actor (Actorは同じ型のコンポーネントを複数持てないため)
			for (const std::shared_ptr<Mesh>& partMesh : partMeshes)
			{
				auto partActor = actorService->Create(L"SponzaPart");
				std::shared_ptr<Renderer> partRenderer = partActor->AddComponent<Renderer>();
				partRenderer->SetMesh(partMesh);
				partRenderer->SetMaterial(sponzaMaterial);
			}
		}
	}

	// スカイボックスの描画
	{
		auto skyboxPass = shaderPassPool->GetShaderPass<SkyboxPass>();
		Material skyBox = materialHandler->Create(skyboxPass);
		skyBox.SetTexturePath(L"SampleGame/Assets/sky_14_2k.png");

		auto skyboxMesh = std::make_shared<Mesh>(device, "SampleGame/Assets/skybox.glb", D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		auto skyboxActor = actorService->Create(L"Skybox");
		skyboxActor->GetComponent<Transform>()->scale = Vec3(300.0f, 300.0f, 300.0f);

		std::shared_ptr<Renderer> skyboxRenderer = skyboxActor->AddComponent<Renderer>();
		skyboxRenderer->SetMesh(skyboxMesh);
		skyboxRenderer->SetMaterial(skyBox);
	}

	auto debugActor = actorService->Create(L"DebugDepth");
	auto debugComponent = debugActor->AddComponent<DepthDebugComponent>();
	debugComponent->Setup(engineService);

	// InputDebugComponent の追加
	auto inputDebugActor = actorService->Create(L"InputDebug");
	auto inputDebugComponent = inputDebugActor->AddComponent<InputDebugComponent>();
	inputDebugComponent->Setup(engineService);
}

void TestWorld::Update(float deltaTime)
{
}

