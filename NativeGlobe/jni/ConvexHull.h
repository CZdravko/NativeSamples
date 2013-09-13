/*
 * ConvexHull.h
 *
 *  Created on: 10. sep. 2013
 *      Author: ZCule
 */

#ifndef CONVEXHULL_H_
#define CONVEXHULL_H_

//count points forming a convex hull vith 2*count line segments
class ConvexHull {
public:
	btVector3 points[];
	int connectivity[];
	float count;


	ConvexHull();
	ConvexHull(btVector3 *points, int count, int *connectivity){
		this -> points = points;
		this -> count = count;
		this -> connectivity = connectivity;
	};

	virtual ~ConvexHull();
};

#endif /* CONVEXHULL_H_ */
