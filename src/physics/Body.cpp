#include "../../include/physics/Body.hpp"
#include <GL/gl.h>

namespace physics {

Body::Body() {
	position.set(0.0f, 0.0f);
	velocity.set(0.0f, 0.0f);
	force.set(0.0f, 0.0f);
	width.set(1.0f, 1.0f);
	terminalVelocity.set(0.0f, 0.0f);

	rotation = 0.0f;
	angularVelocity = 0.0f;
	torque = 0.0f;
	friction = 0.2f;
	mass = FLT_MAX;
	invMass = 0.f;
	I = FLT_MAX;
	invI = 0.0f;
	canJump = false;
	id = 0;
	textureId = 0;
}

void Body::addForce(const Vec2 &f) {
	force += f;
}

void Body::set(const Vec2& w, float m) {
	position.set(0.0f, 0.0f);
	velocity.set(0.0f, 0.0f);
	force.set(0.0f, 0.0f);
	terminalVelocity.set(0.0f, 0.0f);
	width = w;

	rotation = 0.0f;
	angularVelocity = 0.0f;
	torque = 0.0f;
	friction = 0.2f;
	setMass(m);
	canJump = false;
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

void Body::applyTexture(GLuint textureId) {
    this->textureId = textureId;
}

void Body::draw() const {
    // Bind the texture
    glBindTexture(GL_TEXTURE_2D, textureId);

    // Draw the object using the texture
    glBegin(GL_POLYGON);
    glTexCoord2f(0.0f, 0.0f);
    glVertex2f(position.x - width.x / 2.0f, position.y - width.y / 2.0f);

    glTexCoord2f(1.0f, 0.0f);
    glVertex2f(position.x + width.x / 2.0f, position.y - width.y / 2.0f);

    glTexCoord2f(1.0f, 1.0f);
    glVertex2f(position.x + width.x / 2.0f, position.y + width.y / 2.0f);

    glTexCoord2f(0.0f, 1.0f);
    glVertex2f(position.x - width.x / 2.0f, position.y + width.y / 2.0f);
    glEnd();

    // Unbind the texture
    glBindTexture(GL_TEXTURE_2D, 0);
}

}
