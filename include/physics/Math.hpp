#pragma once

#include <cmath>
#include <cassert>

namespace physics {
struct Vec2 {
	float x, y;
	Vec2() {}
	Vec2(float x, float y) : x(x), y(y) {}

	void Set(float x_, float y_) { x = x_; y = y_; }

	Vec2 operator-() { return Vec2(-x, -y); }

	void operator+=(const Vec2& v) {
		x += v.x; y += v.y;
	}

	void operator-=(const Vec2& v) {
		x -= v.x; y -= v.y;
	}

	void operator*=(float a) {
		x *= a; y *= a;
	}

	float Length() const {
		return sqrtf(x * x + y * y);
	}
};

struct Mat22 {
	Vec2 col1, col2;
	Mat22() {}
	Mat22(float angle) {
		float c = cosf(angle), s = sinf(angle);
		col1.x = c; col2.x = -s;
		col1.y = s; col2.y = c;
	}

	Mat22(const Vec2& col1, const Vec2& col2) : col1(col1), col2(col2) {}

	Mat22 Transpose() const {
		return Mat22(Vec2(col1.x, col2.x), Vec2(col1.y, col2.y));
	}

	Mat22 Invert() const {
		float a = col1.x, b = col2.x, c = col1.y, d = col2.y;
		Mat22 B;
		float det = a * d - b * c;
		assert(det != 0.0f);
		det = 1.0f / det;
		B.col1.x =  det * d;	B.col2.x = -det * b;
		B.col1.y = -det * c;	B.col2.y =  det * a;
		return B;
	}
};

float dot(const Vec2& a, const Vec2& b);
float cross(const Vec2& a, const Vec2& b);
Vec2 cross(const Vec2& a, float s);
Vec2 cross(float s, const Vec2& a);
Vec2 operator*(const Mat22& A, const Vec2& v);
Vec2 operator+(const Vec2& a, const Vec2& b);
Vec2 operator-(const Vec2& a, const Vec2& b);
Vec2 operator*(float s, const Vec2& v);
Mat22 operator+(const Mat22& A, const Mat22& B);
Mat22 operator*(const Mat22& A, const Mat22& B);
float abs(float a);
Vec2 abs(const Vec2& a);
Mat22 abs(const Mat22& A);
float sign(float x);
float min(float a, float b);
float max(float a, float b);
float clamp(float a, float low, float high);
template<typename T> void swap(T& a, T& b);
float random();
float random(float lo, float hi);
}
