/*
 * util.h
 *
 *  Created on: 5. sep. 2013
 *      Author: ZCule
 */

#ifndef UTIL_H_
#define UTIL_H_

inline btVector3* minZ(btVector3* A, btVector3* B) {
	return (*A).getZ() > (*B).getZ() ? B : A;
}
inline btVector3* maxZ(btVector3* A, btVector3* B) {
	return (*A).getZ() > (*B).getZ() ? A : B;
}

inline int IntersectSegmentPlane(btVector3 a, btVector3 b, Plane p, float &t,
		btVector3 &q) {
// Compute the t value for the directed line ab intersecting the plane
	btVector3 ab = b - a;
	t = (p.d - p.n->dot(a)) / p.n->dot(ab);
// If t in [0..1] compute and return intersection point
	if (t >= 0.0f && t <= 1.0f) {
		q = a + t * ab;
		return 1;
	}
// Else no intersection
	return 0;
}

inline int IntersectSegmentZPlane(btVector3 a, btVector3 b, float p, float &t,
		btVector3 &q){
	btVector3 planeZVector = new btVector3(0.0f,0.0f,0.1f);
	Plane Z = new Plane(&planeZVector,p);
	return IntersectSegmentPlane(a, b, Z, t, q);
}

inline int compre_floats(const void *a, const void *b){
	float fa = **(float **)a;
	float fb = **(float **)b;

	if (fa < fb) return 1;
	else if (fa == fb) return 0;
	else return -1;
}

#endif /* UTIL_H_ */
