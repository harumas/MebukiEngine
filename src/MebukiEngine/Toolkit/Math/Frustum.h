#pragma once
#include "AABB.h"

// カメラの視錐台 (左・右・下・上・近・遠の6平面)
class Frustum
{
public:
	// 平面の方程式 ax + by + cz + d = 0 を (a, b, c, d) で持つ。法線は視錐台の内側を向く
	XMFLOAT4 planes[6];

	// View * Projection 行列から視錐台を作ります
	static Frustum FromViewProjection(const XMMATRIX& viewProjection);

	// AABBが視錐台と交差している (または内側にある) かを返します
	bool Intersects(const AABB& aabb) const;
};
