#pragma once
#include "Vec3.h" 

// 軸に平行なバウンディングボックス
struct AABB
{
	Vec3 center = { 0, 0, 0 };  // 中心
	Vec3 extents = { 0, 0, 0 }; // 中心から各面までの距離

	// 最小・最大の座標から作ります
	static AABB FromMinMax(const Vec3& min, const Vec3& max);

	// 行列で変換した後のAABBを返します (回転すると元より大きくなる)
	AABB Transform(const XMMATRIX& matrix) const;
};
