#pragma once
#include <string>

// ログを出力するヘルパークラス
// (出力先: Visual Studioの出力ウィンドウ)
class Log
{
public:
	Log() = delete;

	// 通常の情報を出力します
	static void Info(const std::string& message);

	// 警告を出力します
	static void Warning(const std::string& message);

	// エラーを出力します
	static void Error(const std::string& message);

private:
	// レベルの接頭辞と改行を付けて出力します
	static void Write(const char* level, const std::string& message);
};
