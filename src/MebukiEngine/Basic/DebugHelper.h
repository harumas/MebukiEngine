#pragma once

#include <exception>
#include <string>

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

inline void CheckBlobHRESULT(HRESULT result, ID3DBlob* error)
{
	if (SUCCEEDED(result))
		return;

	if (result == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
	{
		::OutputDebugString(L"ファイルが見当たりません\n");
		exit(1);
	}

	if (error == nullptr)
	{
		std::string msg = std::system_category().message(result);
		OutputDebugStringA(msg.c_str());
		OutputDebugStringA("\n");
		exit(1);
	}

	std::string s(static_cast<char*>(error->GetBufferPointer()), error->GetBufferSize());
	s += "\n";
	OutputDebugStringA(s.c_str());

	exit(1);
}
