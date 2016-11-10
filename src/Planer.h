/*
 * Planer.h
 *
 *  Created on: Nov 10, 2016
 *      Author: edi
 */

#ifndef PLANER_H_
#define PLANER_H_

#include "opencv2/core/core.hpp"

using namespace cv;


namespace robosheep {

class Planer {
	Point2f aim;
	int proximity;

public:
	Planer();
	virtual ~Planer();
	int plan(Point2f lastPos, Point2f aktPos);
	void setAim(Point2f newAim);
	bool isRoutePointReached(Point2f pos);

};

} /* namespace robosheep */

#endif /* PLANER_H_ */
