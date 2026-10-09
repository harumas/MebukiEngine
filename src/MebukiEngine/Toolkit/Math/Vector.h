#pragma once
#include "Vec3.h"
#include "Mathf.h"

// ベクトルに対する静的関数をまとめたクラス
class Vector
{
public:
	Vector() = delete;

	// ベクトルの内積を求めます
	static inline float Dot(const Vec3& a, const Vec3& b)
	{
		return a.x * b.x + a.y * b.y + a.z * b.z;
	}

	// ベクトルの外積を求めます
	static inline Vec3 Cross(const Vec3& a, const Vec3& b)
	{
		return { a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x };
	}

	// ベクトルをスケールします
	static inline Vec3 Scale(const Vec3& v, const float scale)
	{
		return { v.x * scale, v.y * scale, v.z * scale };
	}

	// ベクトルの長さを求めます
	static inline float Length(const Vec3& v)
	{
		return std::sqrt(LengthSq(v));
	}

	static inline float LengthSq(const Vec3& v)
	{
		return v.x * v.x + v.y * v.y + v.z * v.z;
	}

	// 正規化したベクトルを返します
	static inline Vec3 Normalize(const Vec3& v)
	{
		float length = Length(v);

		if (length <= 0.0f)
		{
			return Vec3(Vec3::ZERO);
		}

		float invLength = 1.0f / length;

		return { v.x * invLength, v.y * invLength, v.z * invLength };
	}

	// 正規化したベクトルを返します (0ベクトルを渡すとNaNになります)
	static inline Vec3 NormalizeFast(const Vec3& v)
	{
		float invLength = 1.0f / Length(v);

		return { v.x * invLength, v.y * invLength, v.z * invLength };
	}

	// 要素ごとに小さいほうの値を取ったベクトルを返します
	static inline Vec3 Min(const Vec3& a, const Vec3& b)
	{
		return { Mathf::Min(a.x, b.x), Mathf::Min(a.y, b.y), Mathf::Min(a.z, b.z) };
	}

	// 要素ごとに大きいほうの値を取ったベクトルを返します
	static inline Vec3 Max(const Vec3& a, const Vec3& b)
	{
		return { Mathf::Max(a.x, b.x), Mathf::Max(a.y, b.y), Mathf::Max(a.z, b.z) };
	}
};
