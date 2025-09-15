#include "GpuConstants.h"
#include "RegisterType.h"

GpuConstants::GpuConstants() :
	frameData(),
	transformData(),
	frameCB(RegisterType::PerFrame),
	transformCB(RegisterType::PerTransform),
	materialCB(RegisterType::PerMaterial)
{
}

void GpuConstants::SetCameraFrameData(const CameraFrameData& cameraFrameData)
{
	frameData.cameraData = cameraFrameData;
}

void GpuConstants::SetDirectionalLightFrameData(const DirectionalLightFrameData& directionalLightFrameData)
{
	frameData.directionalLightData = directionalLightFrameData;
}

UINT GpuConstants::AddTransformData(const XMFLOAT4X4& transformData)
{
	if (transformCount < MAX_RENDERING_COUNT)
	{
		this->transformData.world[transformCount] = transformData;

		// 現在のインデックスを返してからインクリメント
		return transformCount++;
	}

	// 最大描画数を超えた場合は例外を投げる
	throw std::runtime_error("Exceeded maximum number of transforms");
}

void GpuConstants::SetPointLightFrameData(const PointLightFrameData& pointLightFrameData)
{
	frameData.pointLightData = pointLightFrameData;
}

void GpuConstants::UploadFrameBuffer()
{
	frameCB.UploadBufferData(frameData);
}

void GpuConstants::UploadTransformBuffer()
{
	transformCB.UploadBufferData(transformData);
}

void GpuConstants::UploadMaterialBuffer(const void* buffer, size_t bufferSize, uint32_t offset)
{
	// マテリアル単位のオフセットをバイト単位に変換する 
	uint32_t byteOffset = offset * MAX_SHADER_PROPERTY_SIZE;

	materialCB.UploadBufferData(buffer, bufferSize, byteOffset);
}

void GpuConstants::SetFrameCBV(const GraphicsContext& context) const
{
	frameCB.SetRootConstantBufferView(context);
}

void GpuConstants::SetTransformCBV(const GraphicsContext& context, UINT drawHandle) const
{
	transformCB.SetRootConstantBufferView(context, drawHandle);
}

void GpuConstants::SetMaterialCBV(const GraphicsContext& context, uint32_t materialHandle) const
{
	materialCB.SetRootConstantBufferView(context, materialHandle);
}

void GpuConstants::Reset()
{
	transformCount = 0;
}
