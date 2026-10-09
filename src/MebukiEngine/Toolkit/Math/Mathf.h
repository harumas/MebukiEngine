#pragma once

// 数学関数をまとめた静的クラス
class Mathf
{
public:
	Mathf() = delete;

	// 小さいほうの値を返します
	static inline float Min(float a, float b)
	{
		return (std::min)(a, b);
	}

	// 大きいほうの値を返します
	static inline float Max(float a, float b)
	{
		return (std::max)(a, b);
	}

	// 絶対値を返します
	static inline float Abs(float value)
	{
		return (std::abs)(value);
	}

	// 値をmin以上max以下に収めます
	static inline float Clamp(float value, float min, float max)
	{
		return (std::clamp)(value, min, max);
	}
};
