#include "pch.h"
#include "AABB.h"

AABB AABB::FromMinMax(const Vec3& min, const Vec3& max)
{
	AABB aabb;
	aabb.extents = (max - min) * 0.5f;
	aabb.center = min + aabb.extents;
	return aabb;
}

AABB AABB::Transform(const XMMATRIX& matrix) const
{
	// extentsを変換する
	XMVECTOR newExtents = XMVectorScale(XMVectorAbs(matrix.r[0]), extents.x);
	newExtents = XMVectorAdd(newExtents, XMVectorScale(XMVectorAbs(matrix.r[1]), extents.y));
	newExtents = XMVectorAdd(newExtents, XMVectorScale(XMVectorAbs(matrix.r[2]), extents.z));

	AABB result;
	// centerを変換する 
	result.center = XMVector3Transform(center, matrix);
	result.extents = newExtents;
	return result;
}
