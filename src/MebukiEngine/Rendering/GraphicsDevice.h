#pragma once

class GraphicsDevice
{
public:
	static void	Bind(const winrt::com_ptr<ID3D12Device>& device);
	static ID3D12Device* Get() { return device.get(); }

	GraphicsDevice() = delete;
	~GraphicsDevice() = delete;

private:
	inline static winrt::com_ptr<ID3D12Device> device;
};

