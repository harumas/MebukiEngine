#pragma once

// D3D12 デバイスの薄いラッパー。EngineService に Register して使う。
class GraphicsDevice
{
public:
	explicit GraphicsDevice(winrt::com_ptr<ID3D12Device> device) : device(std::move(device))
	{
	}

	ID3D12Device* Get() const
	{
		return device.get();
	}

	GraphicsDevice(const GraphicsDevice&) = delete;
	GraphicsDevice& operator=(const GraphicsDevice&) = delete;

private:
	winrt::com_ptr<ID3D12Device> device;
};
