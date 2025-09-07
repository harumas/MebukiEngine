// プリコンパイル済みヘッダー
#pragma once

// Windows
#define WIN32_LEAN_AND_MEAN // windows.hから一般的でない機能を除外する
#define IDS_APP_TITLE			103
#define IDC_MEBUKIPROJECT			104
#include <Windows.h>

// DirectX
#include <d3d12.h>
#include <dxgi1_6.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <winrt/base.h>
using namespace DirectX;


// STL
#include <vector>
#include <list>
#include <unordered_map>
#include <set>
#include <stack>
#include <queue>
#include <functional>
#include <string>
#include <fstream>
#include <sstream>
#include <thread>
#include <mutex>
#include <new>
#include <memory>
#include <exception>
#include <type_traits>
#include <typeindex>
#include <tchar.h>

// DirectX
#pragma comment( lib, "dxgi.lib" )
#pragma comment( lib, "d3d12.lib" )
#pragma comment( lib, "d3dcompiler.lib" )

// DirectXTex
//#pragma comment( lib, "DirectXTex.lib" )

// XInput
#pragma comment( lib, "XInput.lib" )

// フレームワーク
#include <d3dx12.h>
#include "Basic/DebugHelper.h"
#include "Basic/EventListener.h"
#include "Basic/WinRTHelper.h"
#include "Basic/UID.h"
#include "Basic/EngineService.h"
#include "Basic/WindowInfo.h"