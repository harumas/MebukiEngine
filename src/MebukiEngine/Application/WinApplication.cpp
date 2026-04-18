#include "WinApplication.h"
#include "pch.h"

int WinApplication::Run(ApplicationProperty& property)
{
	// COMの初期化
	ThrowIfFailed(::CoInitializeEx(nullptr, COINIT_MULTITHREADED));

	// グローバル文字列の初期化
	LoadStringW(property.hInstance, IDS_APP_TITLE, titleName, MAX_LOADSTRING);
	LoadStringW(property.hInstance, IDC_MEBUKIPROJECT, className, MAX_LOADSTRING);

	// 文字列のコピー
	wcscpy_s(titleName, property.titleName);
	wcscpy_s(className, property.windowClassName);

	// ウィンドウクラス作成
	ThrowIfFailed(RegisterWindowClass(windowClass, property.hInstance));

	// ウィンドウ作成
	HWND hwnd = CreateWindowInstance(property);
	windowInfo = { hwnd, property.initialRect.right, property.initialRect.bottom };

	OnInitialize(windowInfo);

	// メッセージの処理
	UINT latestMessage = ProcessMessage(property.hInstance);

	OnDispose();

	if (latestMessage != WM_QUIT)
	{
		// ウィンドウを破棄する
		DestroyWindowInstance(hwnd);
	}

	// クラスを登録解除する
	UnregisterWindowClass();

	// COMのリリース
	::CoUninitialize();

	return 0;
}

ATOM WinApplication::RegisterWindowClass(WNDCLASSEXW& wndClass,
	HINSTANCE hInstance) const
{
	wndClass.cbSize = sizeof(WNDCLASSEXW);
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = WindowProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = hInstance;
	wndClass.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
	wndClass.hIconSm = LoadIcon(hInstance, IDI_APPLICATION);
	wndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wndClass.hbrBackground = reinterpret_cast<HBRUSH>((COLOR_WINDOW + 1));
	wndClass.lpszMenuName = MAKEINTRESOURCEW(IDC_MEBUKIPROJECT);
	wndClass.lpszClassName = className;

	return RegisterClassExW(&wndClass);
}

HWND WinApplication::CreateWindowInstance(
	const ApplicationProperty& property) const
{
	DWORD style = WS_OVERLAPPEDWINDOW;

	// ウィンドウサイズの調整
	RECT windowRect = property.initialRect;
	AdjustWindowRect(&windowRect, style, false);

	// ウィンドウ作成
	HWND hwnd = CreateWindowW(className, titleName, style, CW_USEDEFAULT,
		CW_USEDEFAULT, windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top, nullptr,
		nullptr, windowClass.hInstance, (LPVOID)this);

	if (hwnd == nullptr)
	{
		const DWORD errorCode = GetLastError();
		const std::string msg =
			"CreateWindow failed. ErrorCode: " + std::to_string(errorCode);
		ThrowMessage(msg);
	}

	// ウィンドウ表示
	ShowWindow(hwnd, SW_SHOW);

	return hwnd;
}

UINT WinApplication::ProcessMessage(HINSTANCE hInstance) const
{
	const HACCEL hAccelTable =
		LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MEBUKIPROJECT));

	MSG msg = {};
	do
	{
		// メッセージキューからメッセージを取得
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);

				if (msg.message == WM_QUIT)
					return WM_QUIT;
			}
		}
	}
	while (OnProcess(windowInfo));

	return msg.message;
}

void WinApplication::DestroyWindowInstance(HWND hwnd) const
{
	BOOL succeed = DestroyWindow(hwnd);

	if (!succeed)
	{
		const DWORD errorCode = GetLastError();
		const std::string msg =
			"DestroyWindow failed. ErrorCode: " + std::to_string(errorCode);

		ThrowMessage(msg);
	}
}

void WinApplication::UnregisterWindowClass() const
{
	BOOL succeed =
		UnregisterClassW(windowClass.lpszClassName, windowClass.hInstance);

	if (!succeed)
	{
		const DWORD errorCode = GetLastError();
		const std::string msg =
			"UnregisterClass failed. ErrorCode: " + std::to_string(errorCode);

		ThrowMessage(msg);
	}
}

LRESULT WinApplication::HandleProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	// ImGui などのサブシステムにすべてのメッセージを転送する
	if (OnWin32Message && OnWin32Message(hwnd, message, wparam, lparam))
		return true;

	switch (message)
	{
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		case WM_INPUT:
			OnProcessInput(lparam);
		default:
			return DefWindowProc(hwnd, message, wparam, lparam);
	}
}

// ウィンドウプロシージャ
LRESULT WinApplication::WindowProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	WinApplication* self = GetThis(hwnd, message, lparam);
	return self ? self->HandleProc(hwnd, message, wparam, lparam) : DefWindowProc(hwnd, message, wparam, lparam);

}

WinApplication* WinApplication::GetThis(HWND hwnd, UINT msg, LPARAM lp)
{
	if (msg == WM_NCCREATE)
	{
		auto cs = reinterpret_cast<CREATESTRUCT*>(lp);
		auto self = static_cast<WinApplication*>(cs->lpCreateParams);
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)self);
		return self;
	}

	return reinterpret_cast<WinApplication*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
}
