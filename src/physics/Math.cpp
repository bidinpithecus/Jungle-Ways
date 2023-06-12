#include "../../include/physics/Math.hpp"

namespace physics {

float dot(const Vec2& a, const Vec2& b) {
	return a.x * b.x + a.y * b.y;
}

float cross(const Vec2& a, const Vec2& b) {
	return a.x * b.y - a.y * b.x;
}

Vec2 cross(const Vec2& a, float s) {
	return Vec2(s * a.y, -s * a.x);
}

Vec2 cross(float s, const Vec2& a) {
	return Vec2(-s * a.y, s * a.x);
}

Vec2 operator*(const Mat22& A, const Vec2& v) {
	return Vec2(A.col1.x * v.x + A.col2.x * v.y, A.col1.y * v.x + A.col2.y * v.y);
}

Vec2 operator+(const Vec2& a, const Vec2& b) {
	return Vec2(a.x + b.x, a.y + b.y);
}

Vec2 operator-(const Vec2& a, const Vec2& b) {
	return Vec2(a.x - b.x, a.y - b.y);
}

Vec2 operator*(float s, const Vec2& v) {
	return Vec2(s * v.x, s * v.y);
}

Mat22 operator+(const Mat22& A, const Mat22& B) {
	return Mat22(A.col1 + B.col1, A.col2 + B.col2);
}

Mat22 operator*(const Mat22& A, const Mat22& B) {
	return Mat22(A * B.col1, A * B.col2);
}

float abs(float a) {
	return a > 0.0f ? a : -a;
}

Vec2 abs(const Vec2& a) {
	return Vec2(fabsf(a.x), fabsf(a.y));
}

Mat22 abs(const Mat22& A) {
	return Mat22(abs(A.col1), abs(A.col2));
}

float sign(float x) {
	return x < 0.0f ? -1.0f : 1.0f;
}

float min(float a, float b) {
	return a < b ? a : b;
}

float max(float a, float b) {
	return a > b ? a : b;
}

float clamp(float a, float low, float high) {
	return max(low, min(a, high));
}

// Random number in range [-1,1]
float random() {
	float r = (float) rand();
	r /= (float) RAND_MAX;
	r = 2.0f * r - 1.0f;
	return r;
}

float random(float lo, float hi) {
	float r = (float) rand();
	r /= (float) RAND_MAX;
	r = (hi - lo) * r + lo;
	return r;
}

}
