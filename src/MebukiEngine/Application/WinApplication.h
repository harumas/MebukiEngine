#pragma once
#include <Basic/WindowInfo.h>

#define MAX_LOADSTRING 100

struct ApplicationProperty
{
	HINSTANCE hInstance;
	WCHAR titleName[MAX_LOADSTRING];
	WCHAR windowClassName[MAX_LOADSTRING];
	RECT initialRect;
};

class WinApplication
{
public:
	int Run(ApplicationProperty& property);
	std::function<void(const WindowInfo&)> OnInitialize;
	std::function<int(const WindowInfo&)> OnProcess;
	std::function<void(const LPARAM&)> OnProcessInput;
	std::function<void(void)> OnDispose;
	std::function<bool(HWND, UINT, WPARAM, LPARAM)> OnWin32Message;
	std::function<void(UINT, UINT)> OnResize;

private:
	WNDCLASSEXW windowClass = {};
	WindowInfo windowInfo = {};
	WCHAR titleName[MAX_LOADSTRING] = {};
	WCHAR className[MAX_LOADSTRING] = {};

	ATOM RegisterWindowClass(WNDCLASSEXW& windowClass, HINSTANCE hInstance) const;
	HWND CreateWindowInstance(const ApplicationProperty& property) const;
	UINT ProcessMessage(HINSTANCE hInstance) const;
	void DestroyWindowInstance(HWND hwnd) const;
	void UnregisterWindowClass() const;

	LRESULT CALLBACK HandleProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);
	static WinApplication* GetThis(HWND hwnd, UINT msg, LPARAM lp);
};

