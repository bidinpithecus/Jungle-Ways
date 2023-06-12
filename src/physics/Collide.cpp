#include "../../include/physics/Collide.hpp"

namespace physics {

void flip(FeaturePair& fp) {
	swap(fp.e.inEdge1, fp.e.inEdge2);
	swap(fp.e.outEdge1, fp.e.outEdge2);
}

int clipSegmentToLine(ClipVertex vOut[2], ClipVertex vIn[2], const Vec2& normal, float offset, char clipEdge) {
	// Start with no output points
	int numOut = 0;

	// Calculate the distance of end points to the line
	float distance0 = dot(normal, vIn[0].vec) - offset;
	float distance1 = dot(normal, vIn[1].vec) - offset;

	// If the points are behind the plane
	if (distance0 <= 0.0f) {
		vOut[numOut++] = vIn[0];
	}
	if (distance1 <= 0.0f) {
		vOut[numOut++] = vIn[1];
	}

	// If the points are on different sides of the plane
	if (distance0 * distance1 < 0.0f) {
		// Find intersection point of edge and plane
		float interp = distance0 / (distance0 - distance1);
		vOut[numOut].vec = vIn[0].vec + interp * (vIn[1].vec - vIn[0].vec);
		if (distance0 > 0.0f) {
			vOut[numOut].featurePair = vIn[0].featurePair;
			vOut[numOut].featurePair.e.inEdge1 = clipEdge;
			vOut[numOut].featurePair.e.inEdge2 = NO_EDGE;
		} else {
			vOut[numOut].featurePair = vIn[1].featurePair;
			vOut[numOut].featurePair.e.outEdge1 = clipEdge;
			vOut[numOut].featurePair.e.outEdge2 = NO_EDGE;
		}
		++numOut;
	}

	return numOut;
}

void computeIncidentEdge(ClipVertex c[2], const Vec2& h, const Vec2& pos, const Mat22& Rot, const Vec2& normal) {
	// The normal is from the reference box. Convert it
	// to the incident boxe's frame and flip sign.
	Mat22 RotT = Rot.transpose();
	Vec2 n = -(RotT * normal);
	Vec2 nAbs = abs(n);

	if (nAbs.x > nAbs.y) {
		if (sign(n.x) > 0.0f) {
			c[0].vec.set(h.x, -h.y);
			c[0].featurePair.e.inEdge2 = EDGE3;
			c[0].featurePair.e.outEdge2 = EDGE4;

			c[1].vec.set(h.x, h.y);
			c[1].featurePair.e.inEdge2 = EDGE4;
			c[1].featurePair.e.outEdge2 = EDGE1;
		} else {
			c[0].vec.set(-h.x, h.y);
			c[0].featurePair.e.inEdge2 = EDGE1;
			c[0].featurePair.e.outEdge2 = EDGE2;

			c[1].vec.set(-h.x, -h.y);
			c[1].featurePair.e.inEdge2 = EDGE2;
			c[1].featurePair.e.outEdge2 = EDGE3;
		}
	} else {
		if (sign(n.y) > 0.0f) {
			c[0].vec.set(h.x, h.y);
			c[0].featurePair.e.inEdge2 = EDGE4;
			c[0].featurePair.e.outEdge2 = EDGE1;

			c[1].vec.set(-h.x, h.y);
			c[1].featurePair.e.inEdge2 = EDGE1;
			c[1].featurePair.e.outEdge2 = EDGE2;
		} else {
			c[0].vec.set(-h.x, -h.y);
			c[0].featurePair.e.inEdge2 = EDGE2;
			c[0].featurePair.e.outEdge2 = EDGE3;

			c[1].vec.set(h.x, -h.y);
			c[1].featurePair.e.inEdge2 = EDGE3;
			c[1].featurePair.e.outEdge2 = EDGE4;
		}
	}

	c[0].vec = pos + Rot * c[0].vec;
	c[1].vec = pos + Rot * c[1].vec;
}

// The normal points from A to B
int collide(Contact* contacts, Body* bodyA, Body* bodyB) {
	// setup
	Vec2 hA = 0.5f * bodyA->width;
	Vec2 hB = 0.5f * bodyB->width;

	Vec2 posA = bodyA->position;
	Vec2 posB = bodyB->position;

	Mat22 RotA(bodyA->rotation), RotB(bodyB->rotation);

	Mat22 RotAT = RotA.transpose();
	Mat22 RotBT = RotB.transpose();

	Vec2 dp = posB - posA;
	Vec2 dA = RotAT * dp;
	Vec2 dB = RotBT * dp;

	Mat22 C = RotAT * RotB;
	Mat22 absC = abs(C);
	Mat22 absCT = absC.transpose();

	// Box A faces
	Vec2 faceA = abs(dA) - hA - absC * hB;
	if (faceA.x > 0.0f || faceA.y > 0.0f) {
		return 0;
	}
	// Box B faces
	Vec2 faceB = abs(dB) - absCT * hA - hB;
	if (faceB.x > 0.0f || faceB.y > 0.0f) {
		return 0;
	}
	// Find best axis
	Axis axis;
	float separation;
	Vec2 normal;

	// Box A faces
	axis = FACE_A_X;
	separation = faceA.x;
	normal = dA.x > 0.0f ? RotA.col1 : -RotA.col1;

	const float relativeTol = 0.95f;
	const float absoluteTol = 0.01f;

	if (faceA.y > relativeTol * separation + absoluteTol * hA.y) {
		axis = FACE_A_Y;
		separation = faceA.y;
		normal = dA.y > 0.0f ? RotA.col2 : -RotA.col2;
	}

	// Box B faces
	if (faceB.x > relativeTol * separation + absoluteTol * hB.x) {
		axis = FACE_B_X;
		separation = faceB.x;
		normal = dB.x > 0.0f ? RotB.col1 : -RotB.col1;
	}

	if (faceB.y > relativeTol * separation + absoluteTol * hB.y) {
		axis = FACE_B_Y;
		separation = faceB.y;
		normal = dB.y > 0.0f ? RotB.col2 : -RotB.col2;
	}

	// setup clipping plane data based on the separating axis
	Vec2 frontNormal, sideNormal;
	ClipVertex incidentEdge[2];
	float front, negSide, posSide;
	char negEdge, posEdge;

	// Compute the clipping lines and the line segment to be clipped.
	switch (axis) {
		case FACE_A_X: {
			frontNormal = normal;
			front = dot(posA, frontNormal) + hA.x;
			sideNormal = RotA.col2;
			float side = dot(posA, sideNormal);
			negSide = -side + hA.y;
			posSide =  side + hA.y;
			negEdge = EDGE3;
			posEdge = EDGE1;
			computeIncidentEdge(incidentEdge, hB, posB, RotB, frontNormal);
		}
		break;

		case FACE_A_Y: {
			frontNormal = normal;
			front = dot(posA, frontNormal) + hA.y;
			sideNormal = RotA.col1;
			float side = dot(posA, sideNormal);
			negSide = -side + hA.x;
			posSide =  side + hA.x;
			negEdge = EDGE2;
			posEdge = EDGE4;
			computeIncidentEdge(incidentEdge, hB, posB, RotB, frontNormal);
		}
		break;

		case FACE_B_X: {
			frontNormal = -normal;
			front = dot(posB, frontNormal) + hB.x;
			sideNormal = RotB.col2;
			float side = dot(posB, sideNormal);
			negSide = -side + hB.y;
			posSide =  side + hB.y;
			negEdge = EDGE3;
			posEdge = EDGE1;
			computeIncidentEdge(incidentEdge, hA, posA, RotA, frontNormal);
		}
		break;

		case FACE_B_Y: {
			frontNormal = -normal;
			front = dot(posB, frontNormal) + hB.y;
			sideNormal = RotB.col1;
			float side = dot(posB, sideNormal);
			negSide = -side + hB.x;
			posSide =  side + hB.x;
			negEdge = EDGE2;
			posEdge = EDGE4;
			computeIncidentEdge(incidentEdge, hA, posA, RotA, frontNormal);
		}
		break;
	}

	// clip other face with 5 box planes (1 face plane, 4 edge planes)

	ClipVertex clipPoints1[2];
	ClipVertex clipPoints2[2];
	int np;

	// Clip to box side 1
	np = clipSegmentToLine(clipPoints1, incidentEdge, -sideNormal, negSide, negEdge);

	if (np < 2) {
		return 0;
	}
	// Clip to negative box side 1
	np = clipSegmentToLine(clipPoints2, clipPoints1,  sideNormal, posSide, posEdge);

	if (np < 2) {
		return 0;
	}

	// Now clipPoints2 contains the clipping points.
	// Due to roundoff, it is possible that clipping removes all points.

	int numContacts = 0;
	for (int i = 0; i < 2; ++i) {
		float separation = dot(frontNormal, clipPoints2[i].vec) - front;

		if (separation <= 0) {
			contacts[numContacts].separation = separation;
			contacts[numContacts].normal = normal;
			// slide contact point onto reference face (easy to cull)
			contacts[numContacts].position = clipPoints2[i].vec - separation * frontNormal;
			contacts[numContacts].feature = clipPoints2[i].featurePair;
			if (axis == FACE_B_X || axis == FACE_B_Y) {
				flip(contacts[numContacts].feature);
			}
			++numContacts;
		}
	}

	return numContacts;
}

}
