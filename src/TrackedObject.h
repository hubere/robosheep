/*
 * TrackedObject.h
 *
 *  Created on: Oct 26, 2016
 *      Author: edi
 */

#ifndef SRC_TRACKEDOBJECT_H_
#define SRC_TRACKEDOBJECT_H_

#include "TrackedColorBlob.h"
#include "GUI.h"
#include <opencv2/highgui/highgui.hpp>

using namespace cv;


class TrackedObject {

	vector<TrackedColorBlob> colorBlobs;
	vector<Point2i> positions;
	Point2f direction;

public:
	TrackedObject();
	virtual ~TrackedObject();
	void show(GUI& gui);
	vector<TrackedColorBlob> getColorBlobs();
	Point2i getAktualPos();
	Point2i getAktualPosMean();
	vector<Point2i> getPositionHistory();
	void setAktualPos(Point2i);
	void setDirection(Point2f v);
	void toString();
	int getKurswinkelDegree();

	static void onChange(int v, void *ptr) {
		TrackedObject *that = (TrackedObject*) ptr;
		that->refresh();
	}
	void refresh();
};

#endif /* SRC_TRACKEDOBJECT_H_ */
