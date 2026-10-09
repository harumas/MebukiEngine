#include "pch.h"
#include "Frustum.h"

Frustum Frustum::FromViewProjection(const XMMATRIX& viewProjection)
{
	// row-vector規約 (v * M) では、出力の各成分(x',y',z',w')を作る係数は
	// 行列の「列」になる。XMMATRIXのr[]は「行」なので、転置して列を取り出す。
	const XMMATRIX m = XMMatrixTranspose(viewProjection);
	const XMVECTOR c0 = m.r[0]; // x' の係数
	const XMVECTOR c1 = m.r[1]; // y' の係数
	const XMVECTOR c2 = m.r[2]; // z' の係数
	const XMVECTOR c3 = m.r[3]; // w' の係数

	Frustum frustum;

	// クリップ空間で画面内に収まる条件 (D3Dの深度は0~w) を、そのまま平面の式にする
	// 左:  w' + x' >= 0    右:  w' - x' >= 0
	// 下:  w' + y' >= 0    上:  w' - y' >= 0
	// 近:  z'      >= 0    遠:  w' - z' >= 0
	XMStoreFloat4(&frustum.planes[0], XMVectorAdd(c3, c0));
	XMStoreFloat4(&frustum.planes[1], XMVectorSubtract(c3, c0));
	XMStoreFloat4(&frustum.planes[2], XMVectorAdd(c3, c1));
	XMStoreFloat4(&frustum.planes[3], XMVectorSubtract(c3, c1));
	XMStoreFloat4(&frustum.planes[4], c2);
	XMStoreFloat4(&frustum.planes[5], XMVectorSubtract(c3, c2));

	// 法線(a,b,c)の長さで正規化する (距離の計算を正しくするため)
	for (XMFLOAT4& plane : frustum.planes)
	{
		const XMVECTOR p = XMLoadFloat4(&plane);
		const XMVECTOR length = XMVector3Length(p); // (a,b,c)部分の長さ。wは無視される
		XMStoreFloat4(&plane, XMVectorDivide(p, length));
	}

	return frustum;
}

bool Frustum::Intersects(const AABB& aabb) const
{
	for (const XMFLOAT4& plane : planes)
	{
		// 平面までの中心の符号付き距離 (ax + by + cz + d)
		const float centerDistance = plane.x * aabb.center.x + plane.y * aabb.center.y + plane.z * aabb.center.z + plane.w;

		// AABBが法線方向にどれだけ張り出しているか (半径)
		const float radius = std::abs(plane.x) * aabb.extents.x + std::abs(plane.y) * aabb.extents.y + std::abs(plane.z) * aabb.extents.z;

		// 半径を足しても平面の外側なら、AABB全体がこの平面の外側にある
		if (centerDistance + radius < 0.0f)
		{
			return false;
		}
	}

	return true;
}
