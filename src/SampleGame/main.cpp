#include "TestWorld.h"
#include <Toolkit/World.h>

int APIENTRY wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPTSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

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
