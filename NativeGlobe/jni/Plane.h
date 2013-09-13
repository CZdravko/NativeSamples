/*
 * Plane.h
 *
 *  Created on: 9. sep. 2013
 *      Author: ZCule
 */

#ifndef PLANE_H_
#define PLANE_H_

class Plane {
public:

	btVector3 *n;
	GLfloat d;

	Plane();
	Plane(btVector3 *n, GLfloat d);
	virtual ~Plane();
};

#endif /* PLANE_H_ */
