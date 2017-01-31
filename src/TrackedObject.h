/*
 * TrackedObject.h
 *
 *  Created on: Oct 26, 2016
 *      Author: edi
 */

#ifndef SRC_TRACKEDOBJECT_H_
#define SRC_TRACKEDOBJECT_H_

#include "TrackedColorBlob.h"

#include <opencv2/highgui/highgui.hpp>

using namespace cv;


class TrackedObject {

	vector<TrackedColorBlob> colorBlobs;
	vector<Point2i> positions;

public:
	TrackedObject();
	virtual ~TrackedObject();
	void show();
	vector<TrackedColorBlob> getColorBlobs();
	Point2i getAktualPos();
	Point2i getAktualPosMean();
	void setAktualPos(Point2i);
	void print();
	static void onChange(int v, void *ptr) {
		TrackedObject *that = (TrackedObject*) ptr;
		that->doOnChange(v);
	}
	void doOnChange(int v);
};

#endif /* SRC_TRACKEDOBJECT_H_ */
