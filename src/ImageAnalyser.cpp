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
#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;


static const string WINDOW_IMAGE_ANALYSER = "ImageAnalyser - inRange";
static const string WINDOW_CONTOURS = "ImageAnalyser - contours";

// the adjustable parameter
const char* trackbar_range = "range";
int userInputThresholdRange = 0;

int const max_value = 255;
int const max_type = 4;
int const max_BINARY_value = 255;

Mat cnt_img;

// callback and callback parameters
void adjustParameters(int, void*);
void mouseCallBackFunc(int event, int x, int y, int flags, void* userdata);

ImageAnalyser::ImageAnalyser() {
	pTrackedObject = NULL;
	imageToAnalyse = Mat();
	algorithm = ALGORITHM_DETECTBYCONTOURS;
}

ImageAnalyser::~ImageAnalyser() {
}

void ImageAnalyser::show(GUI& gui) {
	gui.addWindow(WINDOW_IMAGE_ANALYSER);
	gui.addWindow(WINDOW_CONTOURS);

	/// Create Trackbar to choose type of Threshold
	createTrackbar(trackbar_range, WINDOW_IMAGE_ANALYSER, &userInputThresholdRange,
			max_value, adjustParameters, this);
}

bool ImageAnalyser::detectObjectPosition() {
	if (pTrackedObject != NULL)
		return detectObjectPosition(imageToAnalyse, *pTrackedObject);
	return false;
}

bool ImageAnalyser::detectObjectPosition(Mat& frame,
		TrackedObject& trackedObject) {
	if (algorithm == ALGORITHM_DETECTBYMOMENTS)
		return detectByMoments(frame, trackedObject);
	else
		return detectByContours(frame, trackedObject);

}

/**
 * Find center of colorBlob,
 */
Point2f ImageAnalyser::detectObjectPosition(Mat &frame,
		TrackedColorBlob& colorBlob) {

	Point2f center;
	float radius;

	vector<vector<Point> > contours = findCountours(frame, colorBlob);
	if (contours.size() == 0) return Point2f();

	int bestIdx = findBestCountour(contours, colorBlob.getSize());
	if (bestIdx == -1)	return Point2f();

	minEnclosingCircle(contours[bestIdx], center, radius);

	// draw
	for (uint i = 0; i < contours.size(); i++) {
		drawContours(analysedImg, contours, i, Scalar(128, 255, 255), 1);
	}
	circle(analysedImg, center, 10, Scalar(0, 255, 255), 4, 8, 0);
	drawContours(analysedImg, contours, bestIdx, Scalar(255, 128, 255), 3);
	imshow(WINDOW_CONTOURS, analysedImg);

	return center;
}

bool ImageAnalyser::detectByContours(Mat &frame, TrackedObject& trackedObject) {

	// store parameters for callback function
	frame.copyTo(imageToAnalyse);
	pTrackedObject = &trackedObject;

	vector<TrackedColorBlob> colorBlobs = trackedObject.getColorBlobs();
	TrackedColorBlob colorBlob = colorBlobs[0];

	vector<vector<Point> > contours = findCountours(frame, colorBlob);

	int bestContourIdx = findBestCountour(contours, colorBlob.getSize());
//	int bestContourIdx = findBestCountourWithMaximumArea(contours);

//
// draw contours into image cnt_img
//
	frame.copyTo(cnt_img);

	for (uint i = 0; i < contours.size(); i++) {
		drawContours(cnt_img, contours, i, Scalar(128, 255, 255), 1);
	}

	if (bestContourIdx == -1) {
		printf("Could not find rightCountourIdx\n");
		return true;
	}

	//
	// find center
	//
	Point2f center;
	float radius;
	minEnclosingCircle(contours[bestContourIdx], center, radius);
	circle(cnt_img, center, 10, Scalar(0, 255, 255), 4, 8, 0);

	drawContours(cnt_img, contours, bestContourIdx, Scalar(255, 128, 255), 3);

	if (colorBlobs.size() > 1) {
		colorBlob = colorBlobs[1];
		contours = findCountours(frame, colorBlob);
		int bestContour2Idx = findBestCountour(contours, colorBlob.getSize());

		for (uint i = 0; i < contours.size(); i++) {
			drawContours(cnt_img, contours, i, Scalar(128, 255, 255), 1);
		}

		if (bestContour2Idx == -1) {
			printf("Could not find bestContour2Idx\n");
			return true;
		}

		drawContours(cnt_img, contours, bestContour2Idx, Scalar(255, 128, 255),
				3);

		Point2f center2;
		minEnclosingCircle(contours[bestContour2Idx], center2, radius);
		circle(cnt_img, center2, 10, Scalar(255, 0, 0), 4, 8, 0);

		//
		// calculate direction
		//
		Point2f v;
		v.x = center2.x - center.x;
		v.y = center2.y - center.y;
		// rotate vector 90 degrees
		float temp = v.y;
		v.y = -v.x;
		v.x = temp;

		//
		// update tracked objects position
		//
		trackedObject.setAktualPos(center);
		trackedObject.setDirection(v);

		Point2f startPoint = (center + center2) * .5;
		Point2f endPoint = startPoint + v;
		line(cnt_img, startPoint, endPoint, cvScalar(255, 0, 255), 3);

	} else {
		//
		// update tracked objects position
		//
		trackedObject.setAktualPos(center);

		//
		// show contours image cnt_img
		//
		ostringstream text;
		text << "contours: " << contours.size();
		putText(cnt_img, text.str(), Point(40, 100), FONT_HERSHEY_COMPLEX_SMALL,
				1, Scalar::all(255), 1, 8);

	}

	imshow(WINDOW_CONTOURS, cnt_img);

	setMouseCallback(WINDOW_CONTOURS, mouseCallBackFunc, &cnt_img);

	return true;
}

/**
 *
 */
vector<vector<Point> > ImageAnalyser::findCountours(Mat &frame,
		TrackedColorBlob& colorBlob) {

	// either set threshold range to user input or to the range given by colorBlob
	int range = colorBlob.getColorRange();
	if (userInputThresholdRange != 0) {
		range = userInputThresholdRange;
	}

	// see http://www.instructables.com/id/How-to-Track-your-Robot-with-OpenCV/step17/OpenCV-Selecting-Your-Color/
	OpenCVUtils utils;
	Scalar treshLow = utils.gimpValue2OpenCV(colorBlob.getGimpColor(), -range);
	Scalar treshHi = utils.gimpValue2OpenCV(colorBlob.getGimpColor(), range);

	//
	// prepare image for finding contours
	//
	Mat imgHSV;
	Mat imgThreshed;
	cvtColor(frame, imgHSV, CV_BGR2HSV); // change to HSV color space
	inRange(imgHSV, treshLow, treshHi, imgThreshed); // threshold
	imshow(WINDOW_IMAGE_ANALYSER, imgThreshed);

	//
	// find contours
	//
	vector<vector<Point> > contours0;
	vector<Vec4i> hierarchy;
	findContours(imgThreshed, contours0, hierarchy, RETR_TREE,
			CHAIN_APPROX_SIMPLE);

	if (contours0.size() == 0)	{
		cout << "ImageAnalyser::findCountours Could not find any contour for treshLow=" << treshLow << " treshHi=" << treshHi << endl;
//		printf("ImageAnalyser::findCountours Could not find any contour for %1$s.\n", colorBlob.toString().c_str());
	}

	return contours0;
}

/**
 * find bestContour with maximum area
 */
int ImageAnalyser::findBestCountourWithMaximumArea(
		vector<vector<Point> > &contour) {

	int rightCountourIdx = -1;
	vector<Point> bestContour1;
	double maxArea = 0;
	for (unsigned int i = 0; i < contour.size(); i++) {
		double area = contourArea(contour[i]);
		if (area > maxArea) {
			maxArea = area;
			rightCountourIdx = i;
			bestContour1 = contour[i];
		}
	}
	return rightCountourIdx;
}

/**
 * Approximate contours to polygons + get bounding rects and circles.
 * Find contour that is closest in size to objSize.
 */
int ImageAnalyser::findBestCountour(vector<vector<Point> > &contour,
		Size objSize) {
	vector<Point> bestContour;
	vector<vector<Point> > contours(contour.size());
	vector<Rect> boundRect(contours.size());
	vector<Point2f> center(contours.size());
	vector<float> radius(contours.size());
	int rightCountourIdx = -1;
	float minRadius = fmin(objSize.width, objSize.height) / 2.0;
	float maxRadius = fmax(objSize.width, objSize.height) * 2.0;
	for (unsigned int i = 0; i < contours.size(); i++) {
		approxPolyDP(Mat(contour[i]), contours[i], 3, true);
		boundRect[i] = boundingRect(Mat(contours[i]));
		minEnclosingCircle(contours[i], center[i], radius[i]);
		if (radius[i] > minRadius and radius[i] < maxRadius) {
			rightCountourIdx = i;
			bestContour = contours[i];
		}
	}

//	//
//	// draw contours into image cnt_img
//	//
//	Mat cnt_img;
//	analysedImg.copyTo(cnt_img);
//
//	RNG rng(12345);
//	//	 TODO FIXME merge: 	int _levels = 3;
//	Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255),
//			rng.uniform(0, 255));
//	for (uint i = 0; i < contours.size(); i++) {
//		drawContours(cnt_img, contours, i, Scalar(128, 255, 255), 1);
////	 TODO FIXME merge: 	drawContours(cnt_img, contours, i, Scalar(128, 255, 255), 1, CV_AA,
//				//hierarchy, std::abs(_levels));
////		circle(cnt_img, center[i], (int) radius[i], color, 2, 8, 0);
//	}

	if (rightCountourIdx == -1) {
		printf("Could not find best contour for size (%1$d/%2$d).\n", objSize.width, objSize.height);
		return -1;
	}

//	drawContours(cnt_img, contours, rightCountourIdx, Scalar(255, 128, 255), 3);
//// TODO FIXME merge: 	drawContours(cnt_img, contours, rightCountourIdx, Scalar(255, 128, 255), 3,	CV_AA, hierarchy, std::abs(_levels));
//	circle(cnt_img, center[rightCountourIdx], 10, color, 4, 8, 0);
//
//	//
//	// show contours image cnt_img
//	//
//	ostringstream text;
//	text << "contours: " << contours.size();
//	putText(cnt_img, text.str(), Point(40, 100), FONT_HERSHEY_COMPLEX_SMALL, 1,
//			Scalar::all(255), 1, 8);
//	imshow(WINDOW_CONTOURS, cnt_img);
//
//	//
//	// update tracked objects position
//	//
//	// TODO FIXME merge: trackedObject.setAktualPos(center[rightCountourIdx]);

	return rightCountourIdx;
}

bool ImageAnalyser::detectByMoments(Mat &frame, TrackedObject& trackedObject) {

	Mat imgHSV;
	Mat imgThreshed;
	Scalar treshColorLow(20, 100, 100);
	Scalar treshColorHi(30, 255, 255);

	// change to HSV color space
	cvtColor(frame, imgHSV, CV_BGR2HSV);

	// treshhold
	inRange(imgHSV, treshColorLow, treshColorHi, imgThreshed);

	// Calculate the moments to estimate sheep position
	Moments mu;
	mu = moments(imgThreshed, false);

	// The actual moment values
	double moment10 = mu.m10;
	double moment01 = mu.m01;
	double area = mu.m00; // cvGetCentralMoment(moments, 0, 0);

	trackedObject.setAktualPos(Point_<int>(moment10 / area, moment01 / area));

	return true;

}

void ImageAnalyser::analyse(std::string imageName,
		TrackedObject& aTrackedObject) {

	/// Load an image
	Mat imageToAnalyse = imread(imageName, 1);
	pTrackedObject = &aTrackedObject;
	imshow(WINDOW_IMAGE_ANALYSER, imageToAnalyse);

	/// Call the function to initialize
	adjustParameters(0, this);

	waitKey(0);
}

void ImageAnalyser::analyse(Mat& frame,
		TrackedObject& aTrackedObject) {
	frame.copyTo(imageToAnalyse);
	pTrackedObject = &aTrackedObject;
	imshow(WINDOW_IMAGE_ANALYSER, frame);

	/// Call the function to initialize
	adjustParameters(0, this);
}



/**
 * @function adjustParameters
 */
void adjustParameters(int, void* callbackObject) {

	ImageAnalyser* pImageAnalyser = static_cast<ImageAnalyser*>(callbackObject);
	pImageAnalyser->detectObjectPosition();
}

void mouseCallBackFunc(int event, int x, int y, int flags, void* userdata) {
	Mat* rgb = (Mat*) userdata;
	if (event == EVENT_LBUTTONDOWN) {
		// cout << "Left button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;

		Mat imgHSV;
		cvtColor(*rgb, imgHSV, CV_BGR2HSV);

		Vec3b p1 = (*rgb).at<Vec3b>(x, y); //Vec3b - Array of 3 uchar numbers
		Vec3b p2 = imgHSV.at<Vec3b>(x, y); //Vec3b - Array of 3 uchar numbers

		//p[0] - H, p[1] - S, p[2] - V
		cout << endl << "Color (RGB): " << (int) p1[0] << ", " << (int) p1[1]
				<< ", " << (int) p1[2] << ")" << endl;
		cout << "Color (HSV): " << (int) p2[0] << ", " << (int) p2[1] << ", "
				<< (int) p2[2] << ")" << endl;

	} else if (event == EVENT_RBUTTONDOWN) {
		cout << "Right button of the mouse is clicked - position (" << x << ", "
				<< y << ")" << endl;
	} else if (event == EVENT_MBUTTONDOWN) {
		cout << "Middle button of the mouse is clicked - position (" << x
				<< ", " << y << ")" << endl;
	} else if (event == EVENT_MOUSEMOVE) {
		// cout << "Mouse move over the window - position (" << x << ", " << y << ")" << endl;
	}
}

