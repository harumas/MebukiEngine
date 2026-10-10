#include "TestWorld.h"
#include <Toolkit/World.h>
#include <filesystem>

namespace
{
	// アセット・シェーダーは src からの相対パスで読み込むため、
	// exeを直接起動した場合でも作業ディレクトリを src に合わせる
	void SetWorkingDirectoryToAssetRoot()
	{
		const std::filesystem::path marker = L"SampleGame/Shaders";

		if (std::filesystem::exists(marker))
		{
			return;
		}

		wchar_t exePath[MAX_PATH];
		if (GetModuleFileNameW(nullptr, exePath, MAX_PATH) == 0)
		{
			return;
		}

		// exeの場所から親ディレクトリを辿って探す
		for (std::filesystem::path dir = std::filesystem::path(exePath).parent_path(); dir.has_relative_path(); dir = dir.parent_path())
		{
			if (std::filesystem::exists(dir / marker))
			{
				std::filesystem::current_path(dir);
				return;
			}
		}
	}
}

int APIENTRY wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPTSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	SetWorkingDirectoryToAssetRoot();

	ApplicationProperty property =
	{
		hInstance,
		L"MebukiEngine",
		L"MebukiEngine",
		{ 0, 0, 1280, 720 }
	};

	WorldProfile profile;
	profile.Register<TestWorld>();

	GameApplication gameApp(property, profile);

	return gameApp.Run();
}
