/*
 * Plane.cpp
 *
 *  Created on: 9. sep. 2013
 *      Author: ZCule
 */

#include "Plane.h"

Plane::Plane() {
	// TODO Auto-generated constructor stub

}

Plane::Plane(btVector3 *n, GLfloat d) {
	this->n = n;
	this->d = d;
}

Plane::~Plane() {
	// TODO Auto-generated destructor stub
}

