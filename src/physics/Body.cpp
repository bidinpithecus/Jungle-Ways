#include "../../include/physics/Body.hpp"

namespace physics {

Body::Body() {
	position.set(0.0, 0.0);
	velocity.set(0.0, 0.0);
	force.set(0.0, 0.0);
	width.set(1.0, 1.0);

	rotation = 0.0;
	angularVelocity = 0.0;
	torque = 0.0;
	friction = 0.2;
	mass = FLT_MAX;
	invMass = 0.0;
	I = FLT_MAX;
	invI = 0.0;
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
	mass = m;

	if (mass < FLT_MAX)	{
		invMass = 1.0 / mass;
		I = mass * (width.x * width.x + width.y * width.y) / 12.0;
		invI = 1.0 / I;
	} else {
		invMass = 0.0f;
		I = FLT_MAX;
		invI = 0.0f;
	}
}

void Body::draw() {
	glPushMatrix();
		glBegin(GL_POLYGON);
			glVertex2f(position.x - width.x, position.y + width.y);
			glVertex2f(position.x + width.x, position.y + width.y);
			glVertex2f(position.x + width.x, position.y - width.y);
			glVertex2f(position.x - width.x, position.y - width.y);
		glEnd();
	glPopMatrix();
}

}
