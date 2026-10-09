#pragma once
#include <chrono>
#include <string>

// 処理時間の計測用。同じ名前の計測は合計時間と回数を積算し、Reportでまとめて出力する
// (出力先: Visual Studioの出力ウィンドウ と 作業ディレクトリの profile.log)
namespace Profiler
{
	// 無効の間はScopedTimerが何も計測しない (毎フレーム計測し続けないようにするため)
	bool IsEnabled();
	void SetEnabled(bool enabled);

	void Add(const std::string& name, double milliseconds);

	// 計測結果以外の情報(メモリ使用量など)を、次のReportに一緒に出力する
	void AddNote(const std::string& text);

	// 積算した結果を出力して、積算をリセットする
	void Report(const std::string& title);
}

// スコープを抜けるまでの時間を計測する
class ScopedTimer
{
public:
	explicit ScopedTimer(const char* name) :
		name(name),
		isActive(Profiler::IsEnabled())
	{
		if (isActive)
			start = std::chrono::steady_clock::now();
	}

	~ScopedTimer()
	{
		if (!isActive)
			return;

		const std::chrono::duration<double, std::milli> elapsed = std::chrono::steady_clock::now() - start;
		Profiler::Add(name, elapsed.count());
	}

	ScopedTimer(const ScopedTimer&) = delete;
	ScopedTimer& operator=(const ScopedTimer&) = delete;

private:
	// 計測しない時に文字列を作らないよう、名前は文字列リテラルを受け取る
	const char* name;
	bool isActive;
	std::chrono::steady_clock::time_point start;
};
