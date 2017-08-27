/*
 * TrackedObject.cpp
 *
 *  Created on: Oct 26, 2016
 *      Author: edi
 */

#include "TrackedObject.h"
#include "OpenCVUtils.h"
#include "ImageAnalyser.h"

#include <stdio.h>
#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;

// names of windows
static const string WINDOW_TRACKED_OBJECT = "Tracked Object";


TrackedObject::TrackedObject() :
		positions(0) {

	// two circles
	// colorBlobs.push_back(TrackedColorBlob(CELLAR_YELLOW_DINA4));
	// colorBlobs.push_back(TrackedColorBlob(GARDEN_BIKE_WHITE));
	colorBlobs.push_back(TrackedColorBlob(GARDEN_BIKE_RED_AND_WHITE));

	cout << "TrackedObject::TrackedObject: created for ";
	for (int i=0;i<colorBlobs.size();i++)
	{
		cout << colorBlobs[i].getName() << endl;
	}
}

TrackedObject::~TrackedObject() {
}

void TrackedObject::show(GUI& gui) {
	for (int i=0;i<colorBlobs.size();i++)
	{
		colorBlobs[i].show(gui);
	}
	gui.addWindow(WINDOW_TRACKED_OBJECT);
}

vector<TrackedColorBlob> TrackedObject::getColorBlobs() {
	return colorBlobs;
}

Point2i TrackedObject::getAktualPos() {
	if (!positions.empty())
		return positions.back();
	return Point2i();
}

Point2i TrackedObject::getAktualPosMean() {
	Scalar s = mean(positions);
	int x = s.val[0];
	int y = s.val[1];
	return Point2i(x, y);
}

void TrackedObject::setDirection(Point2f v)
{
	direction = v;
}

void TrackedObject::setAktualPos(Point_<int> newPos) {

	if (positions.empty()) {
		for (int i = 0; i < 10; i++)
			positions.push_back(newPos);
	}

	// implement ring buffer
	positions.push_back(newPos);
	positions.erase(positions.begin());
}

void TrackedObject::toString() {
	printf(" TrackedObject : positions: ");
	for (std::vector<Point2i>::const_iterator i = positions.begin();
			i != positions.end(); ++i)
		std::cout << *i << ", ";
	cout << "mean=" << getAktualPos() << std::endl;
}


void TrackedObject::refresh() {
//	OpenCVUtils utils;
//	Mat imgHSV;
//
//	Mat imgRgb(140, 170, CV_8UC3, Scalar::all(0));
//
//	imshow(WINDOW_TRACKED_OBJECT, imgRgb);
}

