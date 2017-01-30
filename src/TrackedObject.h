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
	Point2i lastPos;
	Point2i pos;
	vector<Point2i> lastPositions;

public:
	TrackedObject();
	virtual ~TrackedObject();
	void show();
	Size getSize();
	int getColorRange();
	Point2i getLastPos();
	Point2i getAktualPos();
	Point2i setAktualPos(Point2i);
	Scalar getGimpColor();
	void setGimpColor(Scalar gimpColor);
	void print();
};

#endif /* SRC_TRACKEDOBJECT_H_ */
