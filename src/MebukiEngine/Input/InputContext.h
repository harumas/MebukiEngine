#pragma once
#include <bitset>

class InputContext
{
public:
	InputContext(std::bitset<256>* keysCurrent, std::bitset<256>* keysPrevious, std::bitset<5>* mouseCurrent, std::bitset<5>* mousePrevious)
		: keysCurrentBuffer(keysCurrent),
		keysPreviousBuffer(keysPrevious),
		mouseCurrentBuffer(mouseCurrent),
		mousePreviousBuffer(mousePrevious)
	{

	}

	// 現在のフレームと前のフレームの入力状態を指すポインタ 
	std::bitset<256>* keysCurrentBuffer = nullptr;
	std::bitset<256>* keysPreviousBuffer = nullptr;

	std::bitset<5>* mouseCurrentBuffer = nullptr;
	std::bitset<5>* mousePreviousBuffer = nullptr;

	// マウスの移動量を格納する変数
	long mouseDeltaX = 0;
	long mouseDeltaY = 0;
	long mouseWheelDelta = 0;
};

