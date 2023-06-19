#include "../../include/physics/Body.hpp"

namespace physics {

Body::Body() {
	position.set(0.0f, 0.0f);
	velocity.set(0.0f, 0.0f);
	force.set(0.0f, 0.0f);
	width.set(1.0f, 1.0f);

	rotation = 0.0f;
	angularVelocity = 0.0f;
	torque = 0.0f;
	friction = 0.2f;
	mass = FLT_MAX;
	invMass = 0.f;
	I = FLT_MAX;
	invI = 0.0f;
	inTouch = false;
}

void Body::addForce(const Vec2 &f) {
	force += f;
}

void Body::set(const Vec2& w, float m) {
	position.set(0.0f, 0.0f);
	velocity.set(0.0f, 0.0f);
	force.set(0.0f, 0.0f);
	width = w;

	rotation = 0.0f;
	angularVelocity = 0.0f;
	torque = 0.0f;
	friction = 0.2f;
	setMass(m);
	inTouch = false;
}

void Body::setMass(float m) {
	mass = m;

	if (mass < FLT_MAX)	{
		invMass = 1.0f / mass;
		I = mass * (width.x * width.x + width.y * width.y) / 12.0f;
		invI = 1.0f / I;
	} else {
		invMass = 0.0f;
		I = FLT_MAX;
		invI = 0.0f;
	}
}

void Body::draw() {
	Mat22 R(rotation);
	Vec2 x = position;
	Vec2 h = 0.5f * width;

	Vec2 v1 = x + R * Vec2(-h.x, -h.y);
	Vec2 v2 = x + R * Vec2( h.x, -h.y);
	Vec2 v3 = x + R * Vec2( h.x,  h.y);
	Vec2 v4 = x + R * Vec2(-h.x,  h.y);

	glBegin(GL_POLYGON);
		glVertex2f(v1.x, v1.y);
		glVertex2f(v2.x, v2.y);
		glVertex2f(v3.x, v3.y);
		glVertex2f(v4.x, v4.y);
	glEnd();
}

}
