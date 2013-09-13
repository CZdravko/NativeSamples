/*
 * PointLight.h
 *
 *  Created on: 12. sep. 2013
 *      Author: ZCule
 */

#ifndef POINTLIGHT_H_
#define POINTLIGHT_H_

class PointLight {
public:
	int lightIndex;
	float r;
	btVector3 position;
	PointLight();
	virtual ~PointLight();
};

#endif /* POINTLIGHT_H_ */
