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
const char* trackbar_threshold_range   = "range";

int color_H = 0;
int color_S = 0;
int color_V = 0;
int color_range = 0;




/// Function headers
void adjustTrackedObjectParameters(int, void*);


TrackedObject::TrackedObject() : lastPositions(0) {
	theTrackedObject = this;

	// yellow in terms of gimp!
	color_H = 70;
	color_S = 30;
	color_V = 97;
	color_range = 20;
	size = Size(10,15);

	// yellow in cellar
	color_H = 35; //30-40
	color_S = 30; // 30-30
	color_V = 90; // 80-100
	color_range = 20;

	size = Size(100,50);

}

TrackedObject::~TrackedObject() {
}

void TrackedObject::show()
{
	namedWindow(WINDOW_TRACKED_OBJECT, WINDOW_AUTOSIZE);
	moveWindow(WINDOW_TRACKED_OBJECT, 0, 500);

	/// Create Trackbar to choose type of Threshold
	createTrackbar(trackbar_threshold_color_H, WINDOW_TRACKED_OBJECT, &color_H,
			255, adjustTrackedObjectParameters);
	createTrackbar(trackbar_threshold_color_S, WINDOW_TRACKED_OBJECT, &color_S,
			255, adjustTrackedObjectParameters);
	createTrackbar(trackbar_threshold_color_V, WINDOW_TRACKED_OBJECT, &color_V,
			255, adjustTrackedObjectParameters);
	createTrackbar(trackbar_threshold_range, WINDOW_TRACKED_OBJECT, &color_range,
			255, adjustTrackedObjectParameters);
}

Scalar TrackedObject::getGimpColor() {
	return Scalar(color_H, color_S, color_V);
}

void TrackedObject::setGimpColor(Scalar gimpColor) {
	color_H = gimpColor[0];
	color_S = gimpColor[1];
	color_V = gimpColor[2];
}


Size TrackedObject::getSize()
{
	return size;
}

int TrackedObject::getColorRange()
{
	return color_range;
}


Point2i TrackedObject::getLastPos() {
	return lastPos;
}

Point2i TrackedObject::getAktualPos() {
	Scalar s = mean(lastPositions);
	int x = s.val[0];
	int y = s.val[1];
	return Point2i(x,y);
}

Point_<int> TrackedObject::setAktualPos(Point_<int> newPos) {
	lastPos = pos;
	pos = newPos;

	if (lastPositions.empty())
	{
		for (int i=0; i<10; i++)
			lastPositions.push_back(newPos);
	}

	// implement ring buffer
	lastPositions.push_back(newPos);
	lastPositions.erase(lastPositions.begin());

	print();
	return lastPos;
}

void TrackedObject::print()
{
	printf(" TrackedObject : last=(%d,%d) detected=(%d,%d) \n", lastPos.x, lastPos.y, pos.x, pos.y);
	printf(" TrackedObject : lastPositions: ");
	for (std::vector<Point2i>::const_iterator i = lastPositions.begin(); i != lastPositions.end(); ++i)
	    std::cout << *i << ", ";
	cout << "mean=" << getAktualPos() << std::endl;
}


/**
 * @function adjustParameters
 */
void adjustTrackedObjectParameters(int, void*) {

	OpenCVUtils utils;
	Mat imgHSV;

	Mat imgRgb(80,180, CV_8UC3, Scalar::all(0));
	cvtColor(imgRgb, imgHSV, CV_BGR2HSV); // change to HSV color space

	Scalar gimpColor = theTrackedObject->getGimpColor();
	Scalar colorLow = utils.gimpValue2OpenCV(gimpColor, -color_range);
	Scalar colorHigh = utils.gimpValue2OpenCV(gimpColor, color_range);

	rectangle(imgHSV, Point(10,10), Point(60,60), colorLow, -1, 8, 0);
	rectangle(imgHSV, Point(100,10), Point(160,60), colorHigh, -1, 8, 0);

	cvtColor(imgHSV, imgRgb, CV_HSV2BGR); // change back to RGB color space

	imshow(WINDOW_TRACKED_OBJECT, imgRgb);

	// ImageAnalyser::instance().update();
}


