#include "pch.h"
#include "DebugHelper.h"
#include <d3dcompiler.h>

void CheckBlobHRESULT(HRESULT result, ID3DBlob* error)
{
	if (SUCCEEDED(result))
		return;

	if (result == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
	{
		throw std::runtime_error("シェーダファイルが見当たりません");
	}

	if (error == nullptr)
	{
		throw std::runtime_error(std::system_category().message(result));
	}

	std::string s(static_cast<char*>(error->GetBufferPointer()), error->GetBufferSize());
	throw std::runtime_error(s);
}
