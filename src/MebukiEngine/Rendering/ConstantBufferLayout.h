#pragma once

// Constant Bufferの制限は64KB(65536バイト)なので、最大4096個の描画までサポート
constexpr auto MAX_RENDERING_COUNT = 4096;

// 1マテリアルあたり256バイトまで
constexpr auto MAX_SHADER_PROPERTY_SIZE = 256;

// 64マテリアル保持できるようにする
constexpr auto MAX_MATERIAL_COUNT = 64;

struct CameraFrameData
{
	DirectX::XMFLOAT4X4 viewProj;
	alignas(16)	DirectX::XMFLOAT3   cameraPos;
};

struct DirectionalLightFrameData
{
	alignas(16) DirectX::XMFLOAT3 lightDirection;
	alignas(16)	DirectX::XMFLOAT3 lightColor;
	alignas(16)	DirectX::XMFLOAT3 ambientLight;
};

struct PointLightFrameData
{
	alignas(16)	DirectX::XMFLOAT3 lightPosition;
	alignas(16)	DirectX::XMFLOAT3 lightColor;
	alignas(16)	float radius;
};

struct FrameCB
{
	CameraFrameData    cameraData;
	DirectionalLightFrameData directionalLightData;
	PointLightFrameData pointLightData;
};

struct TransformCB
{
	DirectX::XMFLOAT4X4 world[MAX_RENDERING_COUNT];
};

struct MaterialCB
{
	uint8_t data[MAX_SHADER_PROPERTY_SIZE * MAX_MATERIAL_COUNT];
};
