/*
 * Spotlight.h
 *
 *  Created on: 5. sep. 2013
 *      Author: ZCule
 */

#ifndef SPOTLIGHT_H_
#define SPOTLIGHT_H_

class Spotlight {
public:
	Spotlight();
	virtual ~Spotlight();

	btVector3 V, T1, T2, T3, T4, dir;
	GLfloat height, r;
	int lightIndex;
	void test(GlobeApp* ga);
};

#endif /* SPOTLIGHT_H_ */
