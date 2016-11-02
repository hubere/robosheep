/*
 * ImageAnalyser.cpp
 *
 *  Created on: Sep 19, 2012
 *      Author: ubuntu
 */

#include "ImageAnalyser.h"
#include "GUI.h"
#include "OpenCVUtils.h"

#include <stdio.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;
using namespace robosheep;

#define pi 3.14159265

// the adjustable parameter
const char* trackbar_type =
		"Type: \n 0: Binary \n 1: Binary Inverted \n 2: Truncate \n 3: To Zero \n 4: To Zero Inverted";
const char* trackbar_value = "Value";
const char* trackbar_threshold_low = "threshold_low";
const char* trackbar_threshold_high = "threshold_high";

Mat src, src_gray, dst;

int threshold_low = 0;
int threshold_high = 0;

int const max_value = 255;
int const max_type = 4;
int const max_BINARY_value = 255;

/// Function headers
void adjustParameters(int, void*);

ImageAnalyser::ImageAnalyser() {
	pos = Point2f(250, 300);
	lastpos = Point2f(0, 0);
	dir = 45;
	velocity = 1;
	size = Size(10, 15);
	color = Scalar(0, 255, 255); // yellow
}

ImageAnalyser::~ImageAnalyser() {
}

bool ImageAnalyser::detectObjectPosition(Mat& frame) {
	return detectObjectPosition(frame, *trackedObject);
}

bool ImageAnalyser::detectObjectPosition(Mat& frame,
		TrackedObject& trackedObject) {

	OpenCVUtils utils;

	// see http://www.instructables.com/id/How-to-Track-your-Robot-with-OpenCV/step17/OpenCV-Selecting-Your-Color/
	Scalar treshColorLow = utils.gimpValue2OpenCV(trackedObject.getGimpColor(),
			-trackedObject.getColorRange());
	Scalar treshColorHi = utils.gimpValue2OpenCV(trackedObject.getGimpColor(),
			trackedObject.getColorRange());

	//
	// prepare image for finding contours
	//
	Mat imgHSV;
	Mat imgThreshed;
	cvtColor(frame, imgHSV, CV_BGR2HSV); // change to HSV color space
	inRange(imgHSV, treshColorLow, treshColorHi, imgThreshed); // treshhold
	imshow(WINDOW_THRESHED, imgThreshed);

	//
	// find contours
	//
	vector<vector<Point> > contours0;
	vector<Vec4i> hierarchy;
	findContours(imgThreshed, contours0, hierarchy, RETR_TREE,
			CHAIN_APPROX_SIMPLE);

	//
	// find bestContour with maximum area
	//
	vector<Point> bestContour1;
	double maxArea = 0;
	for (unsigned int i = 0; i < contours0.size(); i++) {
		double area = contourArea(contours0[i]);
		if (area > maxArea) {
			maxArea = area;
			bestContour1 = contours0[i];
		}
	}

	// TODO FIXME HU use bestContour only

	//
	// Approximate contours to polygons + get bounding rects and circles
	//
	vector<Point> bestContour2;
	vector<vector<Point> > contours(contours0.size());
	vector<Rect> boundRect(contours.size());
	vector<Point2f> center(contours.size());
	vector<float> radius(contours.size());
	int rightCountourIdx = -1;
	Size objSize = trackedObject.getSize();
	float minRadius = fmin(objSize.width, objSize.height) / 2.0;
	float maxRadius = fmax(objSize.width, objSize.height) * 2.0;
	for (unsigned int i = 0; i < contours.size(); i++) {
		approxPolyDP(Mat(contours0[i]), contours[i], 3, true);
		boundRect[i] = boundingRect(Mat(contours[i]));
		minEnclosingCircle(contours[i], center[i], radius[i]);
		if (radius[i] > minRadius and radius[i] < maxRadius) {
			rightCountourIdx = i;
			bestContour2 = contours[i];
		}
	}

	//
	// draw contours into image cnt_img
	//
	Mat cnt_img;
	frame.copyTo(cnt_img);

	RNG rng(12345);
	int _levels = 3;
	Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255),
			rng.uniform(0, 255));
	for (uint i = 0; i < contours.size(); i++) {
		drawContours(cnt_img, contours, i, Scalar(128, 255, 255), 1, CV_AA,
				hierarchy, std::abs(_levels));
//		circle(cnt_img, center[i], (int) radius[i], color, 2, 8, 0);
	}

	if (rightCountourIdx == -1) {
		printf("Could not find rightCountourIdx\n");
		return true;
	}

	drawContours(cnt_img, contours, rightCountourIdx, Scalar(255, 128, 255), 3,
	CV_AA, hierarchy, std::abs(_levels));
	circle(cnt_img, center[rightCountourIdx], 10, color, 4, 8, 0);

	//
	// show contours image cnt_img
	//
	ostringstream text;
	text << "contours: " << contours0.size();
	putText(cnt_img, text.str(), Point(40,100), FONT_HERSHEY_COMPLEX_SMALL, 2, Scalar::all(255), 3, 8);
	imshow(WINDOW_CONTOURS, cnt_img);

	//
	// update tracked objects position
	//
	trackedObject.setAktualPos(center[rightCountourIdx]);

	return true;
}

void ImageAnalyser::analyse(std::string imageName,
		TrackedObject& aTrackedObject) {

	/// Load an image
	src = imread(imageName, 1);
	imshow(WINDOW_SOURCE, src);

	trackedObject = &aTrackedObject;

	/// Create Trackbar to choose type of Threshold
	createTrackbar(trackbar_threshold_low, WINDOW_THRESHED, &threshold_low,
			max_value, adjustParameters);

	createTrackbar(trackbar_threshold_high, WINDOW_THRESHED, &threshold_high,
			max_value, adjustParameters);

	/// Call the function to initialize
	adjustParameters(0, 0);

	waitKey(0);
}

/**
 * @function adjustParameters
 */
void adjustParameters(int, void*) {

	ImageAnalyser::instance().detectObjectPosition(src);
}

Size ImageAnalyser::getSize() {
	return size;
}

void ImageAnalyser::update() {

	if (pos == Point2f(0, 0))
		return;

	//	radian = degree * (pi/180);
	double radian = dir * (pi / 180);

	double incX = cos(radian);
	double incY = -sin(radian);

	if (dir > 90) {
		incX = cos(pi / 180 - radian);
		incY = sin(pi / 180 - radian);
	}
	if (dir > 180) {
		incX = cos(radian);
		incY = -sin(radian);
	}
	if (dir > 270) {
		incX = cos(radian);
		incY = -sin(radian);
	}

	pos.x += incX * velocity;
	pos.y += incY * velocity;

	if (pos.x < 0)
		pos.x = 0;
	if (pos.y < 0)
		pos.y = 0;
	if (pos.x > 500)
		pos.x = 500;
	if (pos.y > 500)
		pos.y = 500;

	// print();
	adjustParameters(0,0);
}

void ImageAnalyser::draw(cv::Mat &frame) {
	if (pos == Point2f(0, 0))
		return;
	rectangle(frame, cvPoint(pos.x - size.width, pos.y - size.height),
			cvPoint(pos.x + size.width, pos.y + size.height), color, -1, 8, 0);
	if (lastpos == Point2f(0, 0))
		return;
	line(frame, lastpos, pos, cvScalar(255, 0, 0), 2);
	circle(frame, pos, 4, cvScalar(255, 0, 0), 2);
}

Point2f ImageAnalyser::getPosition() {
	return pos;
}

void ImageAnalyser::setPosition(Point2f newPos) {
	pos = newPos;
}

void ImageAnalyser::rotate(double dt) {
	dir += dt;
	if (dir > 360)
		dir = dir - 360;
	if (dir < 0)
		dir = dir + 360;
}

void ImageAnalyser::speedUp() {
	velocity++;
}

void ImageAnalyser::slowDown() {
	velocity--;
}

void ImageAnalyser::print() {
	printf("sheep (%.0f,%.0f): v=%d, d=%d\n", pos.x, pos.y, velocity, dir);
}

