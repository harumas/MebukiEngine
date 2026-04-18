// ImGuiのプリコンパイル済みヘッダーを先に除外してインクルード
#include "ImGuiRenderer.h"

// ImGui コアはプリコンパイル済みヘッダーに含めない
// （ImGuiのファイルはprecompiled headerを使わないためここで直接インクルード）
#include "../../ThirdParty/imgui/imgui.h"
#include "../../ThirdParty/imgui/backends/imgui_impl_win32.h"
#include "../../ThirdParty/imgui/backends/imgui_impl_dx12.h"

void ImGuiRenderer::Initialize(
	ID3D12Device* device,
	ID3D12CommandQueue* commandQueue,
	HWND hwnd,
	UINT frameCount,
	DXGI_FORMAT rtvFormat)
{
	// SRV DescriptorHeap を作成（ImGui 用のフォントテクスチャ用）
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.NumDescriptors = 1;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	winrt::check_hresult(device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS_WRT(srvHeap)));

	// ImGui コンテキストの作成
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	// スタイルの設定
	ImGui::StyleColorsDark();

	// ImGuiのフォントスケールを調整（高DPI対応）
	ImGuiIO& io = ImGui::GetIO();
	io.FontGlobalScale = 1.0f;

	// Win32 バックエンドを初期化
	ImGui_ImplWin32_Init(hwnd);

	// DX12 バックエンドを初期化（1.91.5+ 新API）
	ImGui_ImplDX12_InitInfo dx12Info = {};
	dx12Info.Device = device;
	dx12Info.CommandQueue = commandQueue;
	dx12Info.NumFramesInFlight = static_cast<int>(frameCount);
	dx12Info.RTVFormat = rtvFormat;
	dx12Info.DSVFormat = DXGI_FORMAT_UNKNOWN;
	dx12Info.SrvDescriptorHeap = srvHeap.get();
	dx12Info.LegacySingleSrvCpuDescriptor = srvHeap->GetCPUDescriptorHandleForHeapStart();
	dx12Info.LegacySingleSrvGpuDescriptor = srvHeap->GetGPUDescriptorHandleForHeapStart();
	ImGui_ImplDX12_Init(&dx12Info);
}

void ImGuiRenderer::Shutdown()
{
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void ImGuiRenderer::BeginFrame()
{
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void ImGuiRenderer::EndFrame(ID3D12GraphicsCommandList* commandList)
{
	ImGui::Render();

	// SRV ヒープをセット（描画前にバインドが必要）
	ID3D12DescriptorHeap* heaps[] = { srvHeap.get() };
	commandList->SetDescriptorHeaps(1, heaps);

	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList);
}

bool ImGuiRenderer::ProcessWin32Message(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// ImGui の Win32 メッセージプロシージャを呼ぶ
	extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	return ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam) != 0;
}

ID3D12DescriptorHeap* ImGuiRenderer::GetSrvHeap() const
{
	return srvHeap.get();
}
