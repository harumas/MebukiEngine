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

	// XMFLOAT3から初期化します
	Vec3(const XMFLOAT3& other) : XMFLOAT3(other)
	{}

	// 3DベクトルをXMVECTORから初期化します
	explicit Vec3(const XMVECTOR& other) :XMFLOAT3()
	{
		const XMVECTOR temp = other;
		XMStoreFloat3(this, temp);
	}

	static const XMVECTOR ZERO;
	static const XMVECTOR RIGHT;
	static const XMVECTOR UP;
	static const XMVECTOR FORWARD;

	bool operator==(const Vec3& r) const
	{
		return x == r.x && y == r.y && z == r.z;
	}

	bool operator!=(const Vec3& r) const
	{
		return x != r.x || y != r.y || z != r.z;
	}

	Vec3 operator+(const Vec3& r) const
	{
		return Vec3(x + r.x, y + r.y, z + r.z);
	}

	Vec3 operator-(const Vec3& r) const
	{
		return Vec3(x - r.x, y - r.y, z - r.z);
	}

	Vec3 operator*(const float r) const
	{
		return Vec3(x * r, y * r, z * r);
	}

	Vec3 operator/(const float r) const
	{
		return Vec3(x / r, y / r, z / r);
	}

	// 要素ごとに掛け算します
	Vec3 operator*(const Vec3& r) const
	{
		return Vec3(x * r.x, y * r.y, z * r.z);
	}

	// 要素ごとに割り算します
	Vec3 operator/(const Vec3& r) const
	{
		return Vec3(x / r.x, y / r.y, z / r.z);
	}

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

	void operator*=(const float r)
	{
		this->x *= r;
		this->y *= r;
		this->z *= r;
	}

	void operator/=(const float r)
	{
		this->x /= r;
		this->y /= r;
		this->z /= r;
	}

	// 要素ごとに掛け算します
	void operator*=(const Vec3& other)
	{
		this->x *= other.x;
		this->y *= other.y;
		this->z *= other.z;
	}

	// 要素ごとに割り算します
	void operator/=(const Vec3& other)
	{
		this->x /= other.x;
		this->y /= other.y;
		this->z /= other.z;
	}

	operator XMVECTOR() const
	{
		return XMLoadFloat3(this);
	}
};

