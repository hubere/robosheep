/*
 * Planer.h
 *
 *  Created on: Nov 10, 2016
 *      Author: edi
 */

#ifndef PLANER_H_
#define PLANER_H_

#include "GUI.h"

#include "opencv2/core/core.hpp"

using namespace cv;


namespace robosheep {

class Planer {
	Point2f aim;
	int proximity;

	int speed;
	int motorSpeed1;
	int motorSpeed2;

public:
	Planer();
	virtual ~Planer();
	void show(GUI& gui);
	void setAim(Point2f newAim);
	bool isRoutePointReached(Point2f pos);
	int plan(Point2f lastPos, Point2f aktPos);
	int getMotorSpeed1();
	int getMotorSpeed2();
};

} /* namespace robosheep */

#endif /* PLANER_H_ */
