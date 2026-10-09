#pragma once
#include <XInput.h>
#include <bitset>
#include <WinUser.h>
#include "InputContext.h"
#include <memory>

class InputProvider
{
public:
	void Initialize(HWND hwnd);
	void Process(const LPARAM& lparam);
	void SwapBuffer();
	void ClearAllKeys();

private:
	HWND hwnd;
	alignas(8) uint8_t rawBuffer[sizeof(RAWINPUT)] = {};

	// キーボード入力バッファ 
	std::bitset<256> keysFirstBuffer = {}, keysSecondBuffer = {};

	// マウス入力バッファ
	std::bitset<5> mouseFirstBuffer = {}, mouseSecondBuffer = {};

	std::shared_ptr<InputContext> context;
};

