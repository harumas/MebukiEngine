#include "GpuConstants.h"
#include "RegisterType.h"

GpuConstants::GpuConstants(ID3D12Device* device) :
	frameData(),
	transformData(),
	frameCB(RegisterType::PerFrame, 1),
	transformCB(RegisterType::PerTransform, MAX_RENDERING_COUNT),
	materialCB(RegisterType::PerMaterial, MAX_RENDERING_COUNT),
	shaderResourceBuffer(device)
{
	descriptorHeaps.emplace_back(shaderResourceBuffer.GetDescriptorHeap());
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
		this->transformData.transforms[transformCount] = TransformData(transformData);

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
	frameCB.UploadBufferData(&frameData);
}

void GpuConstants::UploadTransformBuffer()
{
	transformCB.UploadBufferData(&transformData);
}

void GpuConstants::UploadMaterialBuffer(const void* buffer, size_t bufferSize, uint32_t offset)
{
	// マテリアル単位のオフセットをバイト単位に変換する 
	UINT alignedSize = (bufferSize + 255) & ~255; // 256切り上げ
	uint32_t byteOffset = offset * alignedSize;

	materialCB.UploadBufferData(buffer, alignedSize, byteOffset);
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

UINT GpuConstants::CreateShaderResourceView(ID3D12Resource* resource, DXGI_FORMAT format)
{
	return shaderResourceBuffer.CreateShaderResourceView(resource, format);
}

void GpuConstants::SetGraphicsRootDescriptorTable(const GraphicsContext& context, UINT offset) const
{
	D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = shaderResourceBuffer.GetGPUDescriptorHandle(offset);
	context.SetGraphicsRootDescriptorTable(RegisterType::SRV, gpuHandle);
}

const std::vector<ID3D12DescriptorHeap*>& GpuConstants::GetDescriptorHeaps() const
{
	return descriptorHeaps;
}

void GpuConstants::Reset()
{
	transformCount = 0;
}
