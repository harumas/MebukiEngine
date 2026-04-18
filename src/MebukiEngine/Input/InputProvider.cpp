#include "InputProvider.h"
#include "GameInput.h"

void InputProvider::Initialize(HWND hwnd)
{
	this->hwnd = hwnd;

	RAWINPUTDEVICE rid[2];

	// マウス
	rid[0].usUsagePage = 0x01;
	rid[0].usUsage = 0x02;
	rid[0].dwFlags = 0;
	rid[0].hwndTarget = hwnd;

	// キーボード
	rid[1].usUsagePage = 0x01;
	rid[1].usUsage = 0x06;
	rid[1].dwFlags = 0;
	rid[1].hwndTarget = hwnd;

	BOOL success = RegisterRawInputDevices(rid, 2, sizeof(rid[0]));

	if (success == FALSE)
	{
		DWORD err = GetLastError();
		// ここでブレークポイントを張るか、OutputDebugStringでエラー番号を出力
		// err が 0 以外なら、登録フラグや構造体のサイズ指定が間違っています
	}

	context = std::make_shared<InputContext>(&keysFirstBuffer, &keysSecondBuffer, &mouseFirstBuffer, &mouseSecondBuffer);

	// GameInput クラスにコンテキストを渡す
	GameInput::AttachContext(context);
}

void InputProvider::Process(const LPARAM& lparam)
{
	// ウィンドウがアクティブでない場合は入力を無視する	
	if (GetForegroundWindow() != hwnd)
		return;

	UINT bufferSize = g_bufferSize;

	if (GetRawInputData((HRAWINPUT)lparam, RID_INPUT, rawBuffer, &bufferSize, sizeof(RAWINPUTHEADER)) == (UINT)-1)
		return;

	const RAWINPUT* input = (const RAWINPUT*)rawBuffer;

	// キーボード入力の処理 
	if (input->header.dwType == RIM_TYPEKEYBOARD)
	{
		USHORT vKey = input->data.keyboard.VKey;

		if (vKey < 256)
		{
			// RI_KEY_BREAK フラグ(bit 0)が 1 ならキーが離された状態、0 なら押された状態
			uint64_t state = (input->data.keyboard.Flags & RI_KEY_BREAK) ^ 1;

			int blockIdx = vKey >> 6;
			int bitPos = vKey & 63;
			uint64_t mask = 1ULL << bitPos;
			uint64_t* rawArray = reinterpret_cast<uint64_t*>(context->keysCurrentBuffer);

			rawArray[blockIdx] = (rawArray[blockIdx] & ~mask) | (state << bitPos);
		}
	}
	// マウス入力の処理
	else if (input->header.dwType == RIM_TYPEMOUSE)
	{
		uint32_t flags = input->data.mouse.usButtonFlags;

		// --- 1. 移動量 ---
		// MOUSE_MOVE_ABSOLUTE フラグ(bit 0)が 1 なら絶対座標、0 なら相対座標
		long isRelative = (input->data.mouse.usFlags & 1) ^ 1;

		// 相対座標(1)ならそのまま加算。絶対座標(0)なら 0 を掛けて加算(つまり無視)
		context->mouseDeltaX += input->data.mouse.lLastX * isRelative;
		context->mouseDeltaY += input->data.mouse.lLastY * isRelative;

		// --- 2. ホイール ---
		// RI_MOUSE_WHEEL は bit10
		long hasWheel = (flags >> 10) & 1;

		// ホイール入力があれば加算、なければ 0 を掛けて無視
		context->mouseWheelDelta += static_cast<short>(input->data.mouse.usButtonData) * hasWheel;

		// --- 3. ボタン状態 ---
		// usButtonFlags には bit0〜9 に DOWN(偶数ビット) と UP(奇数ビット) が交互に並んでいる。
		// bit0〜4 に圧縮して抽出する。
		uint8_t downMask =
			((flags >> 0) & 1) |
			((flags >> 1) & 2) |
			((flags >> 2) & 4) |
			((flags >> 3) & 8) |
			((flags >> 4) & 16);

		uint8_t upMask =
			((flags >> 1) & 1) |
			((flags >> 2) & 2) |
			((flags >> 3) & 4) |
			((flags >> 4) & 8) |
			((flags >> 5) & 16);

		uint8_t currentRaw = static_cast<uint8_t>(context->mouseCurrentBuffer->to_ulong());
		uint8_t nextRaw = (currentRaw | downMask) & ~upMask;

		*context->mouseCurrentBuffer = std::bitset<5>(nextRaw);
	}
}

void InputProvider::SwapBuffer()
{
	// バッファを入れ替える
	std::swap(context->keysCurrentBuffer, context->keysPreviousBuffer);
	std::swap(context->mouseCurrentBuffer, context->mousePreviousBuffer);

	// new currentを前フレームの状態でコピー初期化する
	*context->keysCurrentBuffer = *context->keysPreviousBuffer;
	*context->mouseCurrentBuffer = *context->mousePreviousBuffer;

	context->mouseDeltaX = 0;
	context->mouseDeltaY = 0;
	context->mouseWheelDelta = 0;
}

void InputProvider::ClearAllKeys()
{
	context->keysCurrentBuffer->reset();
	context->keysPreviousBuffer->reset();
	context->mouseCurrentBuffer->reset();
	context->mousePreviousBuffer->reset();
}
