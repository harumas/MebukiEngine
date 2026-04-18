#include "GameApplication.h"

GameApplication::GameApplication(const ApplicationProperty& property, const WorldProfile& profile) :
	application(profile),
	appProperty(property)
{
	// アプリケーションイベントのバインド
	winApp.OnInitialize = std::bind(&Application::Initialize, &application, std::placeholders::_1);
	winApp.OnProcess = std::bind(&Application::Process, &application, std::placeholders::_1);
	winApp.OnProcessInput = std::bind(&Application::ProcessInput, &application, std::placeholders::_1);
	winApp.OnDispose = std::bind(&Application::Finalize, &application);

	// Win32 メッセージを ImGui に転送する
	winApp.OnWin32Message = std::bind(&Application::ProcessWin32Message, &application,
		std::placeholders::_1, std::placeholders::_2,
		std::placeholders::_3, std::placeholders::_4);
}

int GameApplication::Run()
{
	return winApp.Run(appProperty);
}
