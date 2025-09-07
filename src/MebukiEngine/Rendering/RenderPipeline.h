#pragma once
#include "RootSignature.h"
#include "RenderTargetBuffer.h"
#include "DepthStencilBuffer.h"
#include "GraphicsContext.h"

class RootSignature;

class RenderPipeline
{
public:
	EventListener<GraphicsContext&> onRenderProcess;

	void Initialize(const WindowInfo& windowInfo);
	void Update(const WindowInfo& windowInfo);
	void Finalize();

	ID3D12RootSignature* GetRootSignature() const;

private:
	static constexpr UINT frameBufferCount = 2;
	winrt::com_ptr<ID3D12Device> device = nullptr;
	winrt::com_ptr<IDXGISwapChain3> swapChain = nullptr;
	winrt::com_ptr<RenderTargetBuffer> renderTargetBuffer = nullptr;
	winrt::com_ptr<DepthStencilBuffer> depthStencilBuffer = nullptr;
	winrt::com_ptr<ID3D12CommandAllocator> commandAllocator[frameBufferCount] = {};
	winrt::com_ptr<ID3D12CommandQueue> commandQueue = nullptr;
	winrt::com_ptr<RootSignature> rootSignature = nullptr;
	winrt::com_ptr<ID3D12PipelineState> pipelineState = nullptr;
	winrt::com_ptr<ID3D12GraphicsCommandList> commandList = nullptr;
	winrt::com_ptr<ID3D12Fence> fence = nullptr;

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
	void WaitForNextFrame();
	void PopulateCommandList();
};
