#include "RenderPipeline.h"
#include "GraphicsDevice.h"
#include "RegisterType.h"

void RenderPipeline::Initialize(const WindowInfo& windowInfo)
{
	// DXGIFactoryの作成
	winrt::com_ptr<IDXGIFactory6> dxgiFactory;
	CreateDXGIFactory(dxgiFactory);

	// D3D12デバイスの作成
	CreateD3D12Device(dxgiFactory.get(), device);
	GraphicsDevice::Bind(device);

	// GPU定数バッファの作成
	gpuConstants = std::make_unique<GpuConstants>(device.get());

	// コマンドキューの作成
	CreateCommandQueue();

	// スワップチェーンの作成
	CreateSwapChain(dxgiFactory.get(), windowInfo);

	// フレームリソースの作成
	CreateFrameResources();

	// デプスステンシルバッファの作成
	depthStencilBuffer = std::make_unique<DepthStencilBuffer>(device.get(), windowInfo.width, windowInfo.height);

	// ルートシグネチャの作成
	CreateRootSignature();

	// コマンドリストの作成
	CreateCommandList();

	// フレーム同期に使用するイベントハンドルを作成する
	CreateFence();
	WaitForFence();
}

void RenderPipeline::RenderFrame(const WindowInfo& windowInfo)
{
	UINT backBufferIndex = renderTargetBuffer->GetBackBufferIndex();
	// コマンドアロケータをリセット
	winrt::check_hresult(commandAllocator[backBufferIndex]->Reset());

	// セットアップをする前にコマンドリストをリセット
	winrt::check_hresult(commandList->Reset(commandAllocator[backBufferIndex].get(), pipelineState.get()));

	// バックバッファをレンダーターゲットとして使用する
	const D3D12_RESOURCE_BARRIER& barrierPresent2RT = renderTargetBuffer->GetResourceBarrier(D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	commandList->ResourceBarrier(1, &barrierPresent2RT);

	CD3DX12_VIEWPORT viewPort(0.0f, 0.0f, windowInfo.width, windowInfo.height);
	CD3DX12_RECT rect(0.0f, 0.0f, windowInfo.width, windowInfo.height);

	commandList->RSSetViewports(1, &viewPort);
	commandList->RSSetScissorRects(1, &rect);

	// ルートシグネチャをセット 
	commandList->SetGraphicsRootSignature(rootSignature->Get());

	// ディスクリプタヒープをセット 
	const auto heaps = gpuConstants->GetDescriptorHeaps();
	commandList->SetDescriptorHeaps(heaps.size(), heaps.data());

	// レンダーターゲットをセット
	const D3D12_CPU_DESCRIPTOR_HANDLE& rtvHandle = renderTargetBuffer->GetRTVHandle();
	const D3D12_CPU_DESCRIPTOR_HANDLE& dsvHandle = depthStencilBuffer->GetDSVHandle();
	commandList->OMSetRenderTargets(1, &rtvHandle, true, &dsvHandle);

	// レンダーターゲットをクリア
	constexpr float backgroundColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };// 赤、緑、青、アルファ
	commandList->ClearRenderTargetView(rtvHandle, backgroundColor, 0, nullptr);

	// デプスステンシルバッファをクリア
	commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	GraphicsContext context(commandList, windowInfo);

	onRenderProcess(context, *gpuConstants);

	// バックバッファは画面更新に使用される
	const D3D12_RESOURCE_BARRIER& barrier2RTPresent = renderTargetBuffer->GetResourceBarrier(D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	commandList->ResourceBarrier(1, &barrier2RTPresent);

	winrt::check_hresult(commandList->Close());

	// コマンドリストの実行
	ID3D12CommandList* commandLists[] = { commandList.get() };
	commandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);

	// フレームバッファを入れ替える 
	winrt::check_hresult(swapChain->Present(1, 0));

	WaitForNextFrame();
}

void RenderPipeline::Finalize()
{
	// GPUが、デストラクタによってクリーンアップされようとしているリソースを
	// 参照しなくなっていることを確認
	WaitForFence();

	// フレーム同期に使用するイベントハンドルを破棄する
	CloseHandle(fenceEvent);
}

ID3D12RootSignature* RenderPipeline::GetRootSignature() const
{
	return rootSignature.get()->Get();
}

void RenderPipeline::CreateDXGIFactory(winrt::com_ptr<IDXGIFactory6>& dxgiFactory)
{
	UINT dxgiFactoryFlags = 0;

#ifdef _DEBUG
	// デバッグレイヤーを有効にする
	winrt::com_ptr<ID3D12Debug> debugLayer;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugLayer))))
	{
		debugLayer->EnableDebugLayer();
		dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
	}
#endif

	// DXGIFactoryの初期化
	winrt::check_hresult(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS_WRT(dxgiFactory)));
}

void RenderPipeline::CreateRootSignature()
{
	rootSignature = std::make_unique<RootSignature>();
	rootSignature->Initialize(device.get(), std::vector<RootParameter>
	{
		{ ParameterType::CBV, RegisterType::PerFrame, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_ALL, false, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC },
		{ ParameterType::CBV, RegisterType::PerTransform, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_ALL, false, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC },
		{ ParameterType::CBV, RegisterType::PerMaterial, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_ALL, false, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC },
		{ ParameterType::SRV, 0, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_PIXEL, true, D3D12_DESCRIPTOR_RANGE_FLAG_NONE },
	});
}

void RenderPipeline::CreateFrameResources()
{
	// レンダーターゲットバッファーの作成
	renderTargetBuffer = std::make_unique<RenderTargetBuffer>(device.get(), swapChain.get(), frameBufferCount);

	//コマンドアロケータを作成 
	for (UINT i = 0; i < frameBufferCount; ++i)
	{
		D3D12_COMMAND_LIST_TYPE commandListType = D3D12_COMMAND_LIST_TYPE_DIRECT;
		winrt::check_hresult(device->CreateCommandAllocator(commandListType, IID_PPV_ARGS_WRT(commandAllocator[i])));
	}
}

void RenderPipeline::CreateFence()
{
	UINT backBufferIndex = renderTargetBuffer->GetBackBufferIndex();
	winrt::check_hresult(device->CreateFence(fenceValue[backBufferIndex], D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS_WRT(fence)));
	fenceValue[backBufferIndex]++;

	// フレーム同期に使用するイベントハンドルを作成する
	fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (fenceEvent == nullptr)
	{
		winrt::check_hresult(HRESULT_FROM_WIN32(GetLastError()));
	}
}

void RenderPipeline::CreateCommandList()
{
	UINT backBufferIndex = renderTargetBuffer->GetBackBufferIndex();
	// コマンドリストの生成
	winrt::check_hresult(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator[backBufferIndex].get(), pipelineState.get(), IID_PPV_ARGS_WRT(commandList)));

	// コマンドリストのクローズ
	// 初期状態がOpenなので、Closeしておく
	winrt::check_hresult(commandList->Close());
}

void RenderPipeline::WaitForFence()
{
	UINT backBufferIndex = renderTargetBuffer->GetBackBufferIndex();
	// キューにシグナルコマンドをスケジュールする
	winrt::check_hresult(commandQueue->Signal(fence.get(), fenceValue[backBufferIndex]));

	// フェンスの処理が終わるまで待つ
	winrt::check_hresult(fence->SetEventOnCompletion(fenceValue[backBufferIndex], fenceEvent));
	WaitForSingleObjectEx(fenceEvent, INFINITE, FALSE);

	// 現在のフレームのフェンス値を増加させる
	fenceValue[backBufferIndex]++;
}

void RenderPipeline::WaitForNextFrame()
{
	UINT backBufferIndex = renderTargetBuffer->GetBackBufferIndex();

	// キューにシグナルコマンドをスケジュールする
	const UINT64 currentValue = fenceValue[backBufferIndex];
	winrt::check_hresult(commandQueue->Signal(fence.get(), currentValue));

	// フレームインデックスを更新
	renderTargetBuffer->RefreshBackBufferIndex();

	// 次のフレームまでGPUを待つ
	if (fence->GetCompletedValue() < fenceValue[backBufferIndex])
	{
		winrt::check_hresult(fence->SetEventOnCompletion(fenceValue[backBufferIndex], fenceEvent));
		WaitForSingleObjectEx(fenceEvent, INFINITE, FALSE);
	}

	// 次のフレームのフェンス値を設定する
	fenceValue[backBufferIndex] = static_cast<UINT>(currentValue) + 1;
}

void RenderPipeline::CreateD3D12Device(IDXGIFactory6* dxgiFactory, winrt::com_ptr<ID3D12Device>& devicePtr)
{
	// グラフィックスボードの選択
	IDXGIAdapter* selectedAdapter = nullptr;
	SIZE_T adapterVideoMemory = 0;

	// VRAMが最大のアダプタを選択する
	IDXGIAdapter* tmpAdapter = nullptr;

	for (int i = 0; SUCCEEDED(dxgiFactory->EnumAdapters(i, &tmpAdapter)); ++i)
	{
		DXGI_ADAPTER_DESC adapterDesc;
		winrt::check_hresult(tmpAdapter->GetDesc(&adapterDesc));

		// 現在選択されているものより小さいグラフィックボードはスキップ
		if (adapterDesc.DedicatedVideoMemory <= adapterVideoMemory)
			continue;

		selectedAdapter = tmpAdapter;
		adapterVideoMemory = adapterDesc.DedicatedVideoMemory;
	}

	// D3D12以上をサポート
	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_12_0;

	// デバイスの作成
	winrt::check_hresult(D3D12CreateDevice(selectedAdapter, featureLevel, IID_PPV_ARGS_WRT(devicePtr)));
}

void RenderPipeline::CreateCommandQueue()
{
	// コマンドキューの設定
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc = {};
	commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE; // タイムアウト無し
	commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT; // コマンドリストと合わせる

	// コマンドキューの作成
	winrt::check_hresult(device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS_WRT(commandQueue)));
}

void RenderPipeline::CreateSwapChain(IDXGIFactory6* dxgiFactory, const WindowInfo& windowInfo)
{
	winrt::com_ptr<IDXGISwapChain1> swapChain1;

	// スワップチェーンの設定
	DXGI_SWAP_CHAIN_DESC1 swapchainDesc = {};
	swapchainDesc.BufferCount = frameBufferCount;
	swapchainDesc.Width = windowInfo.width;
	swapchainDesc.Height = windowInfo.height;
	swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapchainDesc.SampleDesc.Count = 1;

	// スワップチェーンの作成
	winrt::check_hresult(dxgiFactory->CreateSwapChainForHwnd(
		commandQueue.get(),
		windowInfo.hwnd,
		&swapchainDesc,
		nullptr,
		nullptr,
		swapChain1.put()));

	// IDXGISwapChain3に変換
	swapChain1.as(swapChain);
}

