#pragma once

struct Vec3 : public XMFLOAT3
{
	Vec3() = default;
	Vec3(float x, float y, float z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

	// 3DベクトルをXMVECTORから初期化します	
	explicit Vec3(const XMVECTOR& other) :XMFLOAT3()
	{
		const XMVECTOR temp = other;
		XMStoreFloat3(this, temp);
	}

	static const XMVECTOR RIGHT;
	static const XMVECTOR UP;
	static const XMVECTOR FORWARD;

	inline bool operator == (const Vec3& r) const { return x == r.x && y == r.y && z == r.z; }
	inline bool operator != (const Vec3& r) const { return x != r.x || y != r.y || z != r.z; }
	inline XMVECTOR operator *(const float r) const { return Vec3(x * r, y * r, z * r); }
	inline XMVECTOR operator /(const float r) const { return Vec3(x / r, y / r, z / r); }

	// ベクトルの内積を求めます
	float Dot(const Vec3 in) const { return x * in.x + y * in.y + z * in.z; }

	// ベクトルの外積を求めます
	Vec3 Cross(const Vec3 in) const { return { y * in.z - z * in.y, z * in.x - x * in.z, x * in.y - y * in.x }; }

	// ベクトルをスケールします
	Vec3 Scale(const float scale) const { return  { x * scale, y * scale, z * scale }; }


	Vec3& operator=(const XMVECTOR& other)
	{
		const XMVECTOR temp = other;
		XMStoreFloat3(this, temp);
		return *this;
	}

	void operator+=(const Vec3& other)
	{
		this->x += other.x;
		this->y += other.y;
		this->z += other.z;
	}

	void operator-=(const Vec3& other)
	{
		this->x -= other.x;
		this->y -= other.y;
		this->z -= other.z;
	}

	operator XMVECTOR() const
	{
		return XMLoadFloat3(this);
	}

	// ベクトルの長さを求めます
	float Length() const
	{
		return (static_cast<Vec3>(XMVector3Length(XMVECTOR(*this)))).x;
	}

	// ベクトルを正規化します
	void Normalize()
	{
		*this = XMVector3Normalize(XMVECTOR(*this));
	}
};

