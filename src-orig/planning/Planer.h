/*
 * The Planer knows where robosheep wants to head next (setAim(...)). And 
 * it knows the position of the trackedObject. From the difference of these
 * two it will infere streeing commands for robosheep.
 *
 *  Created on: Nov 10, 2016
 *      Author: Edwin Huber
 */

#ifndef PLANER_H_
#define PLANER_H_

#include "opencv2/core/core.hpp"

#include "presentation/GUI.h"
#include "sensors/TrackedObject.h"


using namespace cv;

namespace robosheep {

class Planer {
	GUI* gui;
	Point aim;
	int proximity;
	TrackedObject& trackedObject;
	Mat plannedImage;

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
	bool setAim(Point newAim);
	Point getAim();
	bool isRoutePointReached();
	int getMotorSpeed1();
	int getMotorSpeed2();
	void speedUp();
	void slowDown();
	Mat& getPlannedImage();
};

}  // namespace robosheep

#endif /* PLANER_H_ */
