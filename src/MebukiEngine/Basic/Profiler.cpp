#include "pch.h"
#include "Profiler.h"

namespace
{
	struct Entry
	{
		std::string name;
		double totalMilliseconds = 0.0;
		int count = 0;
	};

	// 最初に計測された順で出力したいので、mapではなくvectorで持つ
	std::vector<Entry> entries;
	std::vector<std::string> notes;
	bool isEnabled = true;
}

bool Profiler::IsEnabled()
{
	return isEnabled;
}

void Profiler::SetEnabled(bool enabled)
{
	isEnabled = enabled;
}

void Profiler::Add(const std::string& name, double milliseconds)
{
	auto it = std::ranges::find(entries, name, &Entry::name);

	if (it == entries.end())
	{
		entries.push_back({ name, 0.0, 0 });
		it = entries.end() - 1;
	}

	it->totalMilliseconds += milliseconds;
	it->count++;
}

void Profiler::AddNote(const std::string& text)
{
	notes.push_back(text);
}

void Profiler::Report(const std::string& title)
{
	std::ostringstream ss;
	ss << "===== " << title << " =====\n";

	for (const Entry& entry : entries)
	{
		char line[256];
		sprintf_s(line, "%-52s total %9.1f ms  avg %8.3f ms  (x%d)\n", entry.name.c_str(), entry.totalMilliseconds, entry.totalMilliseconds / entry.count, entry.count);
		ss << line;
	}

	for (const std::string& note : notes)
	{
		ss << note << "\n";
	}

	const std::string text = ss.str();
	OutputDebugStringA(text.c_str());

	std::ofstream("profile.log", std::ios::app) << text << std::endl;

	entries.clear();
	notes.clear();
}
