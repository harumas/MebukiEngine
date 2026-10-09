#include "FirstPersonCamera.h"
#include <Toolkit/Actor/Actor.h>
#include <Input/GameInput.h>
#include <algorithm>

// ImGui はプリコンパイル済みヘッダーに含めないため直接インクルード
#include "../../ThirdParty/imgui/imgui.h"

FirstPersonCamera::FirstPersonCamera(ActorRef actorRef) : Camera(actorRef)
{}

void FirstPersonCamera::Setup(const EngineService& engineService)
{
	renderPipeline = engineService.Resolve<RenderPipeline>().get();
	postRenderHandle = renderPipeline->onPostRenderProcess.AddListener(std::bind(&FirstPersonCamera::OnPostRender, this, std::placeholders::_1, std::placeholders::_2));
}

void FirstPersonCamera::OnDestroy()
{
	if (renderPipeline && postRenderHandle != static_cast<UINT>(-1))
	{
		renderPipeline->onPostRenderProcess.RemoveListener(postRenderHandle);
	}
}

void FirstPersonCamera::OnUpdate(float deltaTime)
{
	UpdateRotation();
	UpdatePosition(deltaTime);
}

void FirstPersonCamera::UpdateRotation()
{
	// 常にマウスで回すとImGuiを操作できないので、右ボタンを押している間だけ回す
	if (!GameInput::GetMouseButton(MouseButton::Right))
		return;

	constexpr float mouseSensitivity = XM_PI / 180.0f * 0.2f; // 1ピクセルあたり0.2度
	constexpr float maxPitch = XM_PIDIV2 - 0.01f; // 真上・真下でカメラが反転しないように制限

	const float deltaYaw = static_cast<float>(GameInput::GetMouseDeltaX()) * mouseSensitivity;
	const float deltaPitch = static_cast<float>(GameInput::GetMouseDeltaY()) * mouseSensitivity;

	transform->rotation.y += deltaYaw;
	transform->rotation.x = std::clamp(transform->rotation.x + deltaPitch, -maxPitch, maxPitch);
}

void FirstPersonCamera::UpdatePosition(float deltaTime)
{
	constexpr float moveSpeed = 5.0f;      // m/秒
	constexpr float fastMultiplier = 4.0f; // Shift押下時の倍率

	// 視線方向に沿って動く (Camera::GetViewMatrix と同じ回転を使う)
	const XMMATRIX rotationMatrix = XMMatrixRotationQuaternion(XMQuaternionRotationRollPitchYawFromVector(transform->rotation));
	const XMVECTOR forward = XMVector3TransformNormal(Vec3::FORWARD, rotationMatrix);
	const XMVECTOR right = XMVector3TransformNormal(Vec3::RIGHT, rotationMatrix);

	XMVECTOR direction = XMVectorZero();

	if (GameInput::GetKey(Key::W)) direction += forward;
	if (GameInput::GetKey(Key::S)) direction -= forward;
	if (GameInput::GetKey(Key::D)) direction += right;
	if (GameInput::GetKey(Key::A)) direction -= right;
	if (GameInput::GetKey(Key::E)) direction += Vec3::UP;
	if (GameInput::GetKey(Key::Q)) direction -= Vec3::UP;

	// 斜め移動で速くならないように正規化する
	if (XMVectorGetX(XMVector3LengthSq(direction)) <= 0.0f)
		return;

	const float speed = moveSpeed * (GameInput::GetKey(Key::Shift) ? fastMultiplier : 1.0f);
	transform->position = XMVectorAdd(transform->position, XMVector3Normalize(direction) * (speed * deltaTime));
}

void FirstPersonCamera::OnPostRender(const GraphicsContext& /*context*/, GpuConstants& /*gpuConstants*/)
{
	constexpr ImGuiWindowFlags overlayFlags =
		ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove;

	ImGui::SetNextWindowPos(ImVec2(10.0f, 10.0f), ImGuiCond_Always);
	ImGui::SetNextWindowBgAlpha(0.5f);

	ImGui::Begin("Camera Position", nullptr, overlayFlags);
	ImGui::Text("Position: (%.2f, %.2f, %.2f)", transform->position.x, transform->position.y, transform->position.z);
	ImGui::End();
}
