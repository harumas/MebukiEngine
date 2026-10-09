#pragma once

#include <exception>
#include <string>

// Forward declaration to avoid pulling <d3dcompiler.h> into pch.
struct ID3D10Blob;
typedef ID3D10Blob ID3DBlob;

inline void ThrowIfFailed(HRESULT hr)
{
	// 成功していたらそのままreturnする
	if (SUCCEEDED(hr))
		return;

	// hrのエラー内容をthrowする
	char s_str[64] = {};
	sprintf_s(s_str, "HRESULT of 0x%08X", static_cast<UINT>(hr));
	const std::string errorMessage = std::string(s_str);
	throw std::runtime_error(errorMessage);
}

inline void ThrowIfFailed(ATOM atom)
{
	// 成功していたらそのままreturnする
	if (atom != 0)
		return;

	const DWORD errorCode = GetLastError();
	const std::string errorMessage = "failed. ErrorCode: " + std::to_string(errorCode);
	throw std::runtime_error(errorMessage);
}

inline void ThrowMessage(const std::string& message)
{
	throw std::runtime_error(message);
}

void CheckBlobHRESULT(HRESULT result, ID3DBlob* error);
