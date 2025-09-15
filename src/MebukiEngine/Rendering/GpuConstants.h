#pragma once
#include "ConstantBuffer.h"

class GpuConstants
{
public:
	explicit GpuConstants();

	// フレームの定数バッファにデータをセットする関数
	void SetCameraFrameData(const CameraFrameData& cameraFrameData);
	void SetPointLightFrameData(const PointLightFrameData& pointLightFrameData);
	void SetDirectionalLightFrameData(const DirectionalLightFrameData& directionalLightFrameData);

	// モデル行列のデータをバッファに追加する関数 (戻り値はオフセット) 
	UINT AddTransformData(const XMFLOAT4X4& transformData);

	// 各種定数バッファをGPUに転送する関数
	void UploadFrameBuffer();
	void UploadTransformBuffer();
	void UploadMaterialBuffer(const void* buffer, size_t bufferSize, uint32_t offset);

	// 定数バッファビューをルートシグネチャにセットする関数
	void SetFrameCBV(const GraphicsContext& context) const;
	void SetTransformCBV(const GraphicsContext& context, UINT drawHandle) const;
	void SetMaterialCBV(const GraphicsContext& context, uint32_t materialHandle) const;

	void Reset();

private:
	FrameCB frameData;
	TransformCB transformData;
	UINT transformCount = 0;

	ConstantBuffer<FrameCB> frameCB;
	ConstantBuffer<TransformCB> transformCB;
	ConstantBuffer<MaterialCB> materialCB;
};
