#pragma once

#include "Math.hpp"
#include "Body.hpp"
#include "World.hpp"

namespace physics {

struct Joint {
	Mat22 mat;
	Vec2 localAnchor1, localAnchor2;
	Vec2 r1, r2;
	Vec2 bias;
	Vec2 p;
	Body* body1;
	Body* body2;
	float biasFactor;
	float softness;

	Joint();

	void preStep(float invDt);
	void applyImpulse();
	void set(Body* body1, Body* body2, const Vec2& anchor);
	void draw();
};

}
