/*
 * A TrackedObject will be found in an image by the imageAnalyser and get
 * its position and direction.
 *
 *  Created on: Oct 26, 2016
 *      Author: Edwin Huber
 */

#ifndef SRC_TRACKEDOBJECT_H_
#define SRC_TRACKEDOBJECT_H_

#include <opencv2/highgui/highgui.hpp>

#include "TrackedColorBlob.h"
#include "GUI.h"

using namespace cv;

namespace robosheep {

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

}  // namespace robosheep

#endif /* SRC_TRACKEDOBJECT_H_ */
