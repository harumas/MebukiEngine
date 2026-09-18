#pragma once
#include "RootSignature.h"
#include "RenderTargetBuffer.h"
#include "DepthStencilBuffer.h"
#include "GpuConstants.h"
#include "GraphicsContext.h"
#include "ImGuiRenderer.h"

class RootSignature;

class RenderPipeline
{
public:
	EventListener<GraphicsContext&, GpuConstants&> onRenderProcess;
	EventListener<GraphicsContext&, GpuConstants&> onPostRenderProcess;

	void Initialize(const WindowInfo& windowInfo);
	void RenderFrame(const WindowInfo& windowInfo);
	void Resize(UINT width, UINT height);
	void Finalize();

	ID3D12RootSignature* GetRootSignature() const;
	DepthStencilBuffer* GetDepthStencilBuffer() const;
	RenderTargetBuffer* GetRenderTargetBuffer() const;
	ImGuiRenderer& GetImGuiRenderer() const;
	GpuConstants& GetGpuConstants() const { return *gpuConstants; }
	const winrt::com_ptr<ID3D12Device>& GetDevice() const { return device; }

private:
	static constexpr UINT frameBufferCount = 2;
	winrt::com_ptr<ID3D12Device> device = nullptr;
	winrt::com_ptr<IDXGISwapChain3> swapChain = nullptr;
	winrt::com_ptr<ID3D12CommandAllocator> commandAllocator[frameBufferCount] = {};
	winrt::com_ptr<ID3D12CommandQueue> commandQueue = nullptr;
	winrt::com_ptr<ID3D12PipelineState> pipelineState = nullptr;
	winrt::com_ptr<ID3D12GraphicsCommandList> commandList = nullptr;
	winrt::com_ptr<ID3D12Fence> fence = nullptr;
	std::unique_ptr<RenderTargetBuffer> renderTargetBuffer = nullptr;
	std::unique_ptr<DepthStencilBuffer> depthStencilBuffer = nullptr;
	std::unique_ptr<RootSignature> rootSignature = nullptr;
	std::unique_ptr<GpuConstants> gpuConstants = nullptr;
	std::unique_ptr<ImGuiRenderer> imguiRenderer = nullptr;

	UINT fenceValue[frameBufferCount] = {};
	HANDLE fenceEvent = nullptr;

	void CreateD3D12Device(IDXGIFactory6* dxgiFactory, winrt::com_ptr<ID3D12Device>& devicePtr);
	void CreateCommandQueue();
	void CreateSwapChain(IDXGIFactory6* dxgiFactory, const WindowInfo& windowInfo);
	void CreateDXGIFactory(winrt::com_ptr<IDXGIFactory6>& dxgiFactory);
	void CreateRootSignature();
	void CreateFrameResources();
	void CreateFence();
	void CreateCommandList();

	void WaitForFence();
	void WaitForFenceAt(UINT index);
	void WaitForNextFrame();
};
