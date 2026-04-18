#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <memory>

// ImGuiのDirect3D 12 + Win32 バックエンドを管理するクラス
class ImGuiRenderer
{
public:
	ImGuiRenderer() = default;
	~ImGuiRenderer() = default;

	// ImGuiの初期化
	// device       : D3D12デバイス
	// commandQueue : コマンドキュー（テクスチャアップロード用）
	// hwnd         : ウィンドウハンドル
	// frameCount   : スワップチェーンのフレーム数
	// rtvFormat    : レンダーターゲットのフォーマット
	void Initialize(
		ID3D12Device* device,
		ID3D12CommandQueue* commandQueue,
		HWND hwnd,
		UINT frameCount,
		DXGI_FORMAT rtvFormat);

	// ImGuiの終了処理
	void Shutdown();

	// フレーム開始時に呼ぶ (Win32 NewFrame + ImGui::NewFrame)
	void BeginFrame();

	// フレーム終了時に呼ぶ（コマンドリストに描画コマンドを積む）
	void EndFrame(ID3D12GraphicsCommandList* commandList);

	// Win32 メッセージハンドラに渡す
	// return true の場合はエンジン側でメッセージを処理しない
	bool ProcessWin32Message(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	// SRV DescriptorHeap を外部から取得（RenderPipeline で SetDescriptorHeaps に渡す用）
	ID3D12DescriptorHeap* GetSrvHeap() const;

private:
	winrt::com_ptr<ID3D12DescriptorHeap> srvHeap = nullptr;
};
