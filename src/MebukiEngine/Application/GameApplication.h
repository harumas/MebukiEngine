#pragma once
#include "Application.h"
#include "WinApplication.h"
#include "Toolkit/World/WorldProfile.h"

class GameApplication
{
public:
	GameApplication(const ApplicationProperty& property, const WorldProfile& profile);
	int Run();

private:
	WinApplication winApp;
	Application application;
	ApplicationProperty appProperty;
};

