/*
 * Planer.h
 *
 *  Created on: Nov 10, 2016
 *      Author: edi
 */

#ifndef PLANER_H_
#define PLANER_H_

#include "GUI.h"
#include "TrackedObject.h"

#include "opencv2/core/core.hpp"

using namespace cv;


class Planer {
	Point2f aim;
	int proximity;
	TrackedObject& trackedObject;

	int velocity;
	int rotate;
	int motorSpeed1;
	int motorSpeed2;

public:
	Planer(TrackedObject &pTrackedObject);
	virtual ~Planer();

	void show(GUI& gui);
	int plan();

	void show(Mat& frame);
	void putText(Mat& frame, int line, const string& text);
	void putText(Mat& frame, int line, const string& text, Scalar color);
	void setAim(Point2f newAim);
	Point2f getAim();
	bool isRoutePointReached();
	int getMotorSpeed1();
	int getMotorSpeed2();
	void speedUp();
	void slowDown();
};

#endif /* PLANER_H_ */
