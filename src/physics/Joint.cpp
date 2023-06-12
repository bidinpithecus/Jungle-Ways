#include "../../include/physics/Joint.hpp"

namespace physics {

Joint::Joint() {
	body1 = 0;
	body2 = 0;
	p = Vec2(0.0, 0.0);
	biasFactor = 0.2;
	softness = 0.0;
}

void Joint::set(Body* b1, Body* b2, const Vec2& anchor) {
	body1 = b1;
	body2 = b2;

	Mat22 Rot1(body1->rotation);
	Mat22 Rot2(body2->rotation);
	Mat22 Rot1T = Rot1.transpose();
	Mat22 Rot2T = Rot2.transpose();

	localAnchor1 = Rot1T * (anchor - body1->position);
	localAnchor2 = Rot2T * (anchor - body2->position);

	p.set(0.0f, 0.0f);

	softness = 0.0f;
	biasFactor = 0.2f;
}

void Joint::preStep(float invDt) {
	// Pre-compute anchors, mass matrix, and bias.
	Mat22 Rot1(body1->rotation);
	Mat22 Rot2(body2->rotation);

	r1 = Rot1 * localAnchor1;
	r2 = Rot2 * localAnchor2;

	Mat22 K1;
	K1.col1.x = body1->invMass + body2->invMass;
	K1.col2.x = 0.0f;
	K1.col1.y = 0.0f;
	K1.col2.y = body1->invMass + body2->invMass;

	Mat22 K2;
	K2.col1.x =  body1->invI * r1.y * r1.y;
	K2.col2.x = -body1->invI * r1.x * r1.y;
	K2.col1.y = -body1->invI * r1.x * r1.y;
	K2.col2.y =  body1->invI * r1.x * r1.x;

	Mat22 K3;
	K3.col1.x =  body2->invI * r2.y * r2.y;
	K3.col2.x = -body2->invI * r2.x * r2.y;
	K3.col1.y = -body2->invI * r2.x * r2.y;
	K3.col2.y =  body2->invI * r2.x * r2.x;

	Mat22 K = K1 + K2 + K3;
	K.col1.x += softness;
	K.col2.y += softness;

	mat = K.invert();

	Vec2 p1 = body1->position + r1;
	Vec2 p2 = body2->position + r2;
	Vec2 dp = p2 - p1;

	if (World::positionCorrection) {
		bias = -biasFactor * invDt * dp;
	} else {
		bias.set(0.0f, 0.0f);
	}

	if (World::warmStarting) {
		// Apply accumulated impulse.
		body1->velocity -= body1->invMass * p;
		body1->angularVelocity -= body1->invI * cross(r1, p);

		body2->velocity += body2->invMass * p;
		body2->angularVelocity += body2->invI * cross(r2, p);
	} else {
		p.set(0.0f, 0.0f);
	}
}

void Joint::applyImpulse() {
    Vec2 dv = body2->velocity + cross(body2->angularVelocity, r2) - body1->velocity - cross(body1->angularVelocity, r1);

	Vec2 impulse;

	impulse = mat * (bias - dv - softness * p);

	body1->velocity -= body1->invMass * impulse;
	body1->angularVelocity -= body1->invI * cross(r1, impulse);

	body2->velocity += body2->invMass * impulse;
	body2->angularVelocity += body2->invI * cross(r2, impulse);

	p += impulse;
}

void Joint::draw() {
	Body* b1 = body1;
	Body* b2 = body2;

	Mat22 R1(b1->rotation);
	Mat22 R2(b2->rotation);

	Vec2 x1 = b1->position;
	Vec2 p1 = x1 + R1 * localAnchor1;

	Vec2 x2 = b2->position;
	Vec2 p2 = x2 + R2 * localAnchor2;

	glBegin(GL_LINES);
		glVertex2f(x1.x, x1.y);
		glVertex2f(p1.x, p1.y);
		glVertex2f(x2.x, x2.y);
		glVertex2f(p2.x, p2.y);
	glEnd();
}

}
