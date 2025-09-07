#include "GraphicsDevice.h"

void GraphicsDevice::Bind(const winrt::com_ptr<ID3D12Device>& device)
{
	GraphicsDevice::device.copy_from(device.get());
}
