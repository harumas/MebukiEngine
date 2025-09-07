#include "GameApplication.h"

GameApplication::GameApplication(const ApplicationProperty& property, const WorldProfile& profile) :
	application(profile),
	appProperty(property)
{
	// アプリケーションイベントのバインド
	winApp.OnInitialize = std::bind(&Application::Initialize, &application, std::placeholders::_1);
	winApp.OnProcess = std::bind(&Application::Process, &application, std::placeholders::_1);
	winApp.OnDispose = std::bind(&Application::Finalize, &application);
}

int GameApplication::Run()
{
	return winApp.Run(appProperty);
}
