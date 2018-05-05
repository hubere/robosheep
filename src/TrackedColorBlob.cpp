/*
 * TrackedColorBlob.cpp
 *
 *  Created on: Oct 26, 2016
 *      Author: edi
 */

#include "TrackedColorBlob.h"

#include "OpenCVUtils.h"
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <string>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;

namespace robosheep {

static const string WINDOW_TRACKED_COLOR_BLOB = "Tracked Color Blob ";
static TrackedColorBlob* theTrackedObject;

const char* trackbar_threshold_color_high_H = "high_H";
const char* trackbar_threshold_color_high_S = "high_S";
const char* trackbar_threshold_color_high_V = "high_V";
const char* trackbar_threshold_color_low_H = "low_H";
const char* trackbar_threshold_color_low_S = "low_S";
const char* trackbar_threshold_color_low_V = "low_V";
const char* trackbar_threshold_range = "range";

int color_high_H = 0;
int color_high_S = 0;
int color_high_V = 0;
int color_low_H = 0;
int color_low_S = 0;
int color_low_V = 0;
int color_range = 0;



TrackedColorBlob::TrackedColorBlob(int datasetidx) {
	theTrackedObject = this;
	switch (datasetidx) {

	case GARDEN_YELLOW_DINA4: {
		high_H = 70;
		high_S = 30;
		high_V = 97;
		color_range = 20;
		maxSize = Size(10, 15);
		name = "GARDEN_YELLOW_DINA4";
		break;
	}
	case CELLAR_YELLOW_DINA4: {
		high_H = 35; //30-40
		high_S = 30; // 30-30
		high_V = 90; // 80-100
		color_range = 20;
		maxSize = Size(100, 50);
		name = "CELLAR_YELLOW_DINA4";
		break;
	}
	case CELLAR_YELLOW_CIRCLE: {
		high_H = 35; //30-40
		high_S = 30; // 30-30
		high_V = 90; // 80-90
		color_range = 20;
		maxSize = Size(40, 30);
		name = "CELLAR_YELLOW_CIRCLE";
		break;
	}
	case CELLAR_RED_CIRCLE: {
		high_H = 360; //30-40
		high_S = 50; // 30-30
		high_V = 90; // 80-90
		color_range = 20;
		maxSize = Size(40, 30);
		name = "CELLAR_RED_CIRCLE";
		break;
	}

	case GARDEN_YELLOW_CIRLCE: {
						// ru ru2
		high_H = 264;  //264   0
		high_S = 31;   // 31   0
		high_V = 42;   // 42  92
		color_range = 20;
		maxSize = Size(40, 30);
		name = "GARDEN_YELLOW_CIRLCE";
		break;
	}

	case GARDEN_RED_CIRLCE: {
						// ru ru2
		high_H = 310;  //303 327
		high_S = 28;   // 26  31
		high_V = 97;   // 96  98
		color_range = 20;
		maxSize = Size(40, 30);
		name = "GARDEN_RED_CIRLCE";
		break;
	}

	case GARDEN_LED_RED: {
		//		name		   H    S   V   range  size
		init("GARDEN_LED_RED", 352, 72, 48, 20, Size(10,10), Size(40,40));
		break;
	}

	case GARDEN_LED_YELLOW: {
		//		name		   H    S   V   range  size
		init("GARDEN_LED_YELLOW", 54, 23, 95, 20, Size(10,10), Size(40,40));
		break;
	}

	case GARDEN_BIKE_WHITE: {
		//		name		     highH highS highV lowH,lowS,lowV,range,size
		init("GARDEN_BIKE_WHITE",  220,    1,   96, 100,   3,  43,   10, Size(-1,-1), Size(-1,-1));
		break;
	}

	case GARDEN_BIKE_RED: {
		//		name		     highH,highS,highV,lowH,lowS,lowV,range, size
//		init("GARDEN_BIKE_RED",   19,   15,  100,  13,  34,  48,   20, Size(-1,-1));
		init("GARDEN_BIKE_RED",   19,   63,  255,  6,  34,  180,   10, Size(-1,-1), Size(-1,-1));
		break;
	}

	case GARDEN_BIKE_RED_AND_WHITE: {
		//		name		             highH,highS,highV,lowH,lowS,lowV,range, size
		init("GARDEN_BIKE_RED_AND_WHITE",  255,   40,  100,   0,   0,  80,    0, Size(-1,-1), Size(-1,-1));
		break;
	}

	case GARDEN_BLUE_SQUARE: {
		//		name		      highH,highS,highV,lowH,lowS,lowV,range, size
		init("GARDEN_BLUE_SQUARE",  219,   28,   93, 183,  24,  87,   10, Size(5,5), Size(90, 90) );
		break;
	}

	case GARDEN_ORANGE_SQUARE: {
		//		name		        highH,highS,highV,lowH,lowS,lowV,range, minSize, maxSize
		init("GARDEN_ORANGE_SQUARE",   14,   34,   98,  13,  26,  73,   10, Size(5,5), Size(90, 90) );
		break;
	}


	}
}

TrackedColorBlob::TrackedColorBlob(string name, int H, int S, int V, int range, Size pMinSize, Size pMaxSize)
{
	theTrackedObject = this;
	init(name, H,S,V, range, pMinSize, pMaxSize);
}

TrackedColorBlob::TrackedColorBlob(string name, int highH, int highS, int highV, int lowH, int lowS, int lowV, int range, Size pMinSize, Size pMaxSize)
{
	theTrackedObject = this;
	init(name, highH,highS,highV,lowH,lowS,lowV,range, pMinSize, pMaxSize);
}


void TrackedColorBlob::init(string pName, int highH, int highS, int highV, int range, Size pMinSize, Size pMaxSize)
{
	name = pName;
	high_H = highH;
	high_S = highS;
	high_V = highV;
	low_H = -1;
	low_S = -1;
	low_V = -1;
	color_range = range;
	minSize = pMinSize;
	maxSize = pMaxSize;
}

void TrackedColorBlob::init(string pName, int highH, int highS, int highV, int lowH, int lowS, int lowV, int range, Size pMinSize, Size pMaxSize)
{
	name = pName;
	high_H = highH;
	high_S = highS;
	high_V = highV;
	low_H = lowH;
	low_S = lowS;
	low_V = lowV;
	color_range = range;
	minSize = pMinSize;
	maxSize = pMaxSize;
}


TrackedColorBlob::~TrackedColorBlob() {
}

void TrackedColorBlob::show(GUI& gui) {
	gui.addWindow(WINDOW_TRACKED_COLOR_BLOB + name);

	color_high_H = high_H;
	color_high_S = high_S;
	color_high_V = high_V;
	color_low_H = low_H;
	color_low_S = low_S;
	color_low_V = low_V;

	/// Create Trackbar to choose type of Threshold
	createTrackbar(trackbar_threshold_color_high_H, WINDOW_TRACKED_COLOR_BLOB + name, &high_H,
			255, onChange, this);
	createTrackbar(trackbar_threshold_color_high_S, WINDOW_TRACKED_COLOR_BLOB + name, &high_S,
			255, onChange, this);
	createTrackbar(trackbar_threshold_color_high_V, WINDOW_TRACKED_COLOR_BLOB + name, &high_V,
			255, onChange, this);
	createTrackbar(trackbar_threshold_color_low_H, WINDOW_TRACKED_COLOR_BLOB + name, &low_H,
			255, onChange, this);
	createTrackbar(trackbar_threshold_color_low_S, WINDOW_TRACKED_COLOR_BLOB + name, &low_S,
			255, onChange, this);
	createTrackbar(trackbar_threshold_color_low_V, WINDOW_TRACKED_COLOR_BLOB + name, &low_V,
			255, onChange, this);

	createTrackbar(trackbar_threshold_range, WINDOW_TRACKED_COLOR_BLOB + name,
			&color_range, 255, onChange, this);
}

String TrackedColorBlob::getName() {
	return name;
}

Scalar TrackedColorBlob::getGimpColor() {
	return Scalar(high_H, high_S, high_V);
}

Scalar TrackedColorBlob::getGimpColorHigh() {
	return getGimpColorHigh(color_range);
}

Scalar TrackedColorBlob::getGimpColorLow() {
	return getGimpColorLow(color_range);
}

Scalar TrackedColorBlob::getGimpColorHigh(int range) {
	return Scalar(high_H+range, high_S+range, high_V+range);
}

Scalar TrackedColorBlob::getGimpColorLow(int range) {
	return Scalar(low_H-range, low_S-range, low_V-range);
}


Size TrackedColorBlob::getMinSize() {
	return minSize;
}

Size TrackedColorBlob::getMaxSize() {
	return maxSize;
}

int TrackedColorBlob::getColorRange() {
	return color_range;
}

String TrackedColorBlob::toString(){

	std::stringstream ss;
//	ss << "TrackedColorBlob: color: " << name <<" ( " << high_H << "," << high_S << "," << high_V;
	ss << "TrackedColorBlob: colorH: ( " << high_H << "," << high_S << "," << high_V << ") ";
	ss << "colorL: ( " << low_H << "," << low_S << "," << low_V << ") ";
	ss << "color_range: " << color_range << " minSize: " << minSize << " maxSize: " << maxSize << endl;
	std::string s = ss.str();
	return s;
}

void TrackedColorBlob::refresh() {
	OpenCVUtils utils;
	Mat imgHSV;

	Mat imgRgb(140, 170, CV_8UC3, Scalar::all(0));
	cvtColor(imgRgb, imgHSV, CV_BGR2HSV); // change to HSV color space

	Scalar colorLow = utils.gimpValue2OpenCV(getGimpColorLow(), -color_range);
	Scalar colorHigh = utils.gimpValue2OpenCV(getGimpColorHigh(), color_range);
//	Scalar colorLow = getGimpColorLow(0);
//	Scalar colorHigh = getGimpColorHigh(0);

	cout << "colorLow: " << colorLow << endl;
	cout << "colorHigh: " << colorHigh << endl;


	Size minSize = getMinSize();
	Size maxSize = getMaxSize();
	rectangle(imgHSV, Point( 10, 10), Point(90, 90), colorLow, -1, 8, 0);
	rectangle(imgHSV, Point( 10, 10), Point(10  + maxSize.width, 10 + maxSize.height), Scalar( 0, 0, 0), 2, 8, 0); // indicate size

	rectangle(imgHSV, Point(100, 10), Point(190, 90), colorHigh, -1, 8, 0);
	rectangle(imgHSV, Point(100, 10), Point(100 + minSize.width, 10 + minSize.height), Scalar(0,0,0), 2, 8, 0); // indicate size

	cvtColor(imgHSV, imgRgb, CV_HSV2BGR); // change back to RGB color space

	imshow(WINDOW_TRACKED_COLOR_BLOB + name, imgRgb);

}

}  // namespace robosheep
