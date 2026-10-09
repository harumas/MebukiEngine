#include "pch.h"
#include "Log.h"

void Log::Info(const std::string& message)
{
	Write("INFO", message);
}

void Log::Warning(const std::string& message)
{
	Write("WARN", message);
}

void Log::Error(const std::string& message)
{
	Write("ERROR", message);
}

void Log::Write(const char* level, const std::string& message)
{
	const std::string line = std::string("[") + level + "] " + message + "\n";
	OutputDebugStringA(line.c_str());
}
