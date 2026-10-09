#pragma once

// モデル行列(64バイト) + Padding(192バイト) = 256バイト
// 描画ごとにルートCBVのアドレスをずらして1要素だけ参照するので、
// シェーダ側から見えるcbufferは常に256バイト。バッファ全体は64KBを超えてよい
constexpr auto MAX_RENDERING_COUNT = 1024;

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
	alignas(16)	DirectX::XMFLOAT4X4 lightViewProj;
};

struct PointLightFrameData
{
	alignas(16)	DirectX::XMFLOAT3 lightPosition;
	alignas(16)	DirectX::XMFLOAT3 lightColor;
	alignas(16)	float radius;
};

struct alignas(256) FrameData
{
	CameraFrameData    cameraData;
	DirectionalLightFrameData directionalLightData;
	PointLightFrameData pointLightData;
};

struct alignas(256) TransformData
{
	DirectX::XMFLOAT4X4 world;
};

struct TransformArray
{
	TransformData transforms[MAX_RENDERING_COUNT];
};


struct alignas(256)  MaterialPropertyData
{
	uint8_t data[MAX_SHADER_PROPERTY_SIZE];
};
