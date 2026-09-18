#include "DirectionalLight.h"
#include <Rendering/GraphicsContext.h>
#include <Rendering/GpuConstants.h>
#include <Toolkit/Math/Vec3.h>

DirectionalLight::DirectionalLight(ActorRef actorRef) : Component(actorRef)
{}

void DirectionalLight::OnPreDraw(const GraphicsContext& context, GpuConstants& gpuConstants)
{
	const float LIGHT_DISTANCE = 1000.0f;
	XMVECTOR lightDir = XMLoadFloat3(&currentData.lightDirection);
	XMVECTOR eye = XMVectorScale(lightDir, -LIGHT_DISTANCE);

	XMMATRIX view = XMMatrixLookAtLH(eye, Vec3::ZERO, Vec3::UP);
	// シーン全体(Sponzaは約36m x 24m)が影の範囲に収まるサイズ
	XMMATRIX proj = XMMatrixOrthographicLH(50.0f, 50.0f, 0.1f, LIGHT_DISTANCE * 2.0f);

	//ビュー行列をデータに組み込む 
	XMStoreFloat4x4(&currentData.lightViewProj, view * proj);

	gpuConstants.SetDirectionalLightFrameData(currentData);
}

