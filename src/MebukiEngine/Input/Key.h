#pragma once
#include <windows.h>
#include <cstdint>

// キーボードの列挙体
enum class Key : uint8_t
{
	A = 'A', B = 'B', C = 'C', D = 'D', E = 'E', F = 'F', G = 'G',
	H = 'H', I = 'I', J = 'J', K = 'K', L = 'L', M = 'M', N = 'N',
	O = 'O', P = 'P', Q = 'Q', R = 'R', S = 'S', T = 'T', U = 'U',
	V = 'V', W = 'W', X = 'X', Y = 'Y', Z = 'Z',

	Alpha0 = '0', Alpha1 = '1', Alpha2 = '2', Alpha3 = '3', Alpha4 = '4',
	Alpha5 = '5', Alpha6 = '6', Alpha7 = '7', Alpha8 = '8', Alpha9 = '9',

	Return = VK_RETURN, Escape = VK_ESCAPE, Space = VK_SPACE,
	Backspace = VK_BACK, Tab = VK_TAB,

	UpArrow = VK_UP, DownArrow = VK_DOWN,
	RightArrow = VK_RIGHT, LeftArrow = VK_LEFT,

	Shift = VK_SHIFT, Control = VK_CONTROL, Alt = VK_MENU
};

// マウスボタンの列挙体
enum class MouseButton : uint8_t
{
	Left = 0,
	Right = 1,
	Middle = 2,
	X1 = 3,
	X2 = 4
};
