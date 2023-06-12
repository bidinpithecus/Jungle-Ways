#include "../../include/physics/Arbiter.hpp"

namespace physics {

ArbiterKey::ArbiterKey(Body* b1, Body* b2) {
	if (b1 < b2) {
		body1 = b1; body2 = b2;
	} else {
		body1 = b2; body2 = b1;
	}
}

Arbiter::Arbiter(Body* b1, Body* b2) {
	if (b1 < b2) {
		body1 = b1;
		body2 = b2;
	} else {
		body1 = b2;
		body2 = b1;
	}

	numContacts = collide(contacts, body1, body2);

	friction = sqrtf(body1->friction * body2->friction);
}

void Arbiter::update(Contact* newContacts, int numNewContacts) {
	Contact mergedContacts[2];

	for (int i = 0; i < numNewContacts; ++i) {
		Contact* cNew = newContacts + i;
		int k = -1;
		for (int j = 0; j < numContacts; ++j) {
			Contact* cOld = contacts + j;
			if (cNew->feature.value == cOld->feature.value) {
				k = j;
				break;
			}
		}

		if (k > -1) {
			Contact* c = mergedContacts + i;
			Contact* cOld = contacts + k;
			*c = *cNew;
			if (World::warmStarting) {
				c->pNormal = cOld->pNormal;
				c->pTangent = cOld->pTangent;
				c->pNormalBias = cOld->pNormalBias;
			} else {
				c->pNormal = 0.0f;
				c->pTangent = 0.0f;
				c->pNormalBias = 0.0f;
			}
		} else {
			mergedContacts[i] = newContacts[i];
		}
	}

	for (int i = 0; i < numNewContacts; ++i) {
		contacts[i] = mergedContacts[i];
	}

	numContacts = numNewContacts;
}

void Arbiter::preStep(float invDt) {
	const float kAllowedPenetration = 0.01f;
	float kBiasFactor = World::positionCorrection ? 0.2f : 0.0f;

	for (int i = 0; i < numContacts; ++i) {
		Contact* c = contacts + i;

		Vec2 r1 = c->position - body1->position;
		Vec2 r2 = c->position - body2->position;

		float rn1 = dot(r1, c->normal);
		float rn2 = dot(r2, c->normal);
		float kNormal = body1->invMass + body2->invMass;
		kNormal += body1->invI * (dot(r1, r1) - rn1 * rn1) + body2->invI * (dot(r2, r2) - rn2 * rn2);
		c->massNormal = 1.0f / kNormal;

		Vec2 tangent = cross(c->normal, 1.0f);
		float rt1 = dot(r1, tangent);
		float rt2 = dot(r2, tangent);
		float kTangent = body1->invMass + body2->invMass;
		kTangent += body1->invI * (dot(r1, r1) - rt1 * rt1) + body2->invI * (dot(r2, r2) - rt2 * rt2);
		c->massTangent = 1.0f /  kTangent;

		c->bias = -kBiasFactor * invDt * min(0.0f, c->separation + kAllowedPenetration);

		if (World::accumulateImpulses) {
			Vec2 P = c->pNormal * c->normal + c->pTangent * tangent;

			body1->velocity -= body1->invMass * P;
			body1->angularVelocity -= body1->invI * cross(r1, P);

			body2->velocity += body2->invMass * P;
			body2->angularVelocity += body2->invI * cross(r2, P);
		}
	}
}

void Arbiter::applyImpulse() {
	Body* b1 = body1;
	Body* b2 = body2;

	for (int i = 0; i < numContacts; ++i) {
		Contact* c = contacts + i;
		c->r1 = c->position - b1->position;
		c->r2 = c->position - b2->position;

		// Relative velocity at contact
		Vec2 dv = b2->velocity + cross(b2->angularVelocity, c->r2) - b1->velocity - cross(b1->angularVelocity, c->r1);

		// Compute normal impulse
		float vn = dot(dv, c->normal);

		float dPn = c->massNormal * (-vn + c->bias);

		if (World::accumulateImpulses) {
			// Clamp the accumulated impulse
			float Pn0 = c->pNormal;
			c->pNormal = max(Pn0 + dPn, 0.0f);
			dPn = c->pNormal - Pn0;
		} else {
			dPn = max(dPn, 0.0f);
		}

		// Apply contact impulse
		Vec2 pNormal = dPn * c->normal;

		b1->velocity -= b1->invMass * pNormal;
		b1->angularVelocity -= b1->invI * cross(c->r1, pNormal);

		b2->velocity += b2->invMass * pNormal;
		b2->angularVelocity += b2->invI * cross(c->r2, pNormal);

		// Relative velocity at contact
		dv = b2->velocity + cross(b2->angularVelocity, c->r2) - b1->velocity - cross(b1->angularVelocity, c->r1);

		Vec2 tangent = cross(c->normal, 1.0f);
		float vt = dot(dv, tangent);
		float dPt = c->massTangent * (-vt);

		if (World::accumulateImpulses) {
			// Compute friction impulse
			float maxPt = friction * c->pNormal;

			// Clamp friction
			float oldTangentImpulse = c->pTangent;
			c->pTangent = clamp(oldTangentImpulse + dPt, -maxPt, maxPt);
			dPt = c->pTangent - oldTangentImpulse;
		} else {
			float maxPt = friction * dPn;
			dPt = clamp(dPt, -maxPt, maxPt);
		}

		// Apply contact impulse
		Vec2 pTangent = dPt * tangent;

		b1->velocity -= b1->invMass * pTangent;
		b1->angularVelocity -= b1->invI * cross(c->r1, pTangent);

		b2->velocity += b2->invMass * pTangent;
		b2->angularVelocity += b2->invI * cross(c->r2, pTangent);
	}
}

bool operator<(const ArbiterKey& a1, const ArbiterKey& a2) {
	if (a1.body1 < a2.body1) {
		return true;
	}

	if (a1.body1 == a2.body1 && a1.body2 < a2.body2) {
		return true;
	}
	return false;
}

}
