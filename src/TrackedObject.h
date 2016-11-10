/*
 * TrackedObject.h
 *
 *  Created on: Oct 26, 2016
 *      Author: edi
 */

#ifndef SRC_TRACKEDOBJECT_H_
#define SRC_TRACKEDOBJECT_H_

#include <opencv2/highgui/highgui.hpp>

using namespace cv;


class TrackedObject {
	Size size;
	Mat img;

	// the detected position
	Point_<int> lastPos;
	Point_<int> pos;

public:
	TrackedObject();
	virtual ~TrackedObject();
	void show();
	Size getSize();
	int getColorRange();
	Point_<int> getLastPos();
	Point_<int> getAktualPos();
	Point_<int> setAktualPos(Point_<int>);
	Scalar getGimpColor();
	void setGimpColor(Scalar gimpColor);
	void print();
};

#endif /* SRC_TRACKEDOBJECT_H_ */
