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

static TrackedObject* theTrackedObject;

const char* trackbar_threshold_color_H = "H";
const char* trackbar_threshold_color_S = "S";
const char* trackbar_threshold_color_V = "V";
const char* trackbar_threshold_range = "range";

int color_H = 0;
int color_S = 0;
int color_V = 0;
int color_range = 0;


TrackedObject::TrackedObject() :
		positions(0) {
	theTrackedObject = this;

	// two circles
	// colorBlobs.push_back(TrackedColorBlob(CELLAR_YELLOW_DINA4));
	colorBlobs.push_back(TrackedColorBlob(CELLAR_YELLOW_CIRCLE));
	colorBlobs.push_back(TrackedColorBlob(CELLAR_RED_CIRCLE));
}

TrackedObject::~TrackedObject() {
}

void TrackedObject::show() {
	namedWindow(WINDOW_TRACKED_OBJECT, WINDOW_AUTOSIZE);
	moveWindow(WINDOW_TRACKED_OBJECT, 0, 500);

	/// Create Trackbar to choose type of Threshold
	createTrackbar(trackbar_threshold_color_H, WINDOW_TRACKED_OBJECT, &color_H,
			255, onChange, this);
	createTrackbar(trackbar_threshold_color_S, WINDOW_TRACKED_OBJECT, &color_S,
			255, onChange, this);
	createTrackbar(trackbar_threshold_color_V, WINDOW_TRACKED_OBJECT, &color_V,
			255, onChange, this);
	createTrackbar(trackbar_threshold_range, WINDOW_TRACKED_OBJECT,
			&color_range, 255, onChange, this);
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

void TrackedObject::setAktualPos(Point_<int> newPos) {

	if (positions.empty()) {
		for (int i = 0; i < 10; i++)
			positions.push_back(newPos);
	}

	// implement ring buffer
	positions.push_back(newPos);
	positions.erase(positions.begin());
}

void TrackedObject::print() {
	printf(" TrackedObject : positions: ");
	for (std::vector<Point2i>::const_iterator i = positions.begin();
			i != positions.end(); ++i)
		std::cout << *i << ", ";
	cout << "mean=" << getAktualPos() << std::endl;
}


void TrackedObject::doOnChange(int v) {
	OpenCVUtils utils;
	Mat imgHSV;

	Mat imgRgb(140, 170, CV_8UC3, Scalar::all(0));
	cvtColor(imgRgb, imgHSV, CV_BGR2HSV); // change to HSV color space

	Scalar gimpColor = colorBlobs[0].getGimpColor();
	Scalar colorLow = utils.gimpValue2OpenCV(gimpColor, -color_range);
	Scalar colorHigh = utils.gimpValue2OpenCV(gimpColor, color_range);

	Size theSize = colorBlobs[0].getSize();
	rectangle(imgHSV, Point( 10, 10), Point(10  + theSize.width, 10 + theSize.height), colorLow, -1, 8, 0);
	rectangle(imgHSV, Point(100, 10), Point(100 + theSize.width, 10 + theSize.height), colorHigh, -1, 8, 0);

	if (colorBlobs.size() > 1) {

		Scalar gimpColor = colorBlobs[1].getGimpColor();
		Scalar colorLow = utils.gimpValue2OpenCV(gimpColor, -color_range);
		Scalar colorHigh = utils.gimpValue2OpenCV(gimpColor, color_range);
		theSize = colorBlobs[1].getSize();
		rectangle(imgHSV, Point( 10, 80), Point(10 + theSize.width, 80 + theSize.height), colorLow, -1, 8, 0);
		rectangle(imgHSV, Point(100, 80), Point(100+ theSize.width, 80 + theSize.height), colorHigh, -1, 8, 0);
	}

	cvtColor(imgHSV, imgRgb, CV_HSV2BGR); // change back to RGB color space

	imshow(WINDOW_TRACKED_OBJECT, imgRgb);

}

