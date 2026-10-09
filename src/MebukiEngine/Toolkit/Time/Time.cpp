#include "Time.h"

Time::Time()
{
	// 周波数の取得
	QueryPerformanceFrequency(&frequency);
	// 初期カウントの取得
	QueryPerformanceCounter(&lastTime);
}

Time::~Time()
{

}

void Time::Tick()
{
	LARGE_INTEGER currentTime;
	QueryPerformanceCounter(&currentTime);

	// 経過したカウント数を計算
	long long elapsedTicks = currentTime.QuadPart - lastTime.QuadPart;
	lastTime = currentTime;

	// 秒単位に変換 (deltaTime)
	deltaTime = static_cast<double>(elapsedTicks) / static_cast<double>(frequency.QuadPart);

	// 極端に大きな値を防ぐ（デバッグ中のポーズ後など）
	if (deltaTime < 0)
	{
		deltaTime = 0;
	}
}

float Time::GetDeltaTime() const
{
	return static_cast<float>(deltaTime);
}
