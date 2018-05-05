/*
 * ImageAnalyser.cpp
 *
 *  Created on: Sep 19, 2012
 *      Author: ubuntu
 */

#include "ImageAnalyser.h"
#include "GUI.h"
#include "Garden.h"
#include "OpenCVUtils.h"

#include <stdio.h>
#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;


static const string WINDOW_IA_ORIG = "ImageAnalyser - original";
static const string WINDOW_IA_INRANGE = "ImageAnalyser - inRange";
static const string WINDOW_IA_CONTOURS = "ImageAnalyser - contours";
bool showOrig = false;
bool showInrange = false;

// the adjustable parameter
const char* trackbar_range = "range";
int userInputThresholdRange = 0;

int const max_value = 255;
int const max_type = 4;
int const max_BINARY_value = 255;

Point lastRightClickPosition = Point();
int radius= 10;

// callback and callback parameters
void adjustParameters(int, void*);
void mouseCallBackFuncOrig(int event, int x, int y, int flags, void* userdata);
void mouseCallBackFuncContours(int event, int x, int y, int flags, void* userdata);

ImageAnalyser::ImageAnalyser() {
	pTrackedObject = NULL;
	imageToAnalyse = Mat();
	analysedImg = Mat();
	algorithm = ALGORITHM_DETECTBYCONTOURS;
}

ImageAnalyser::~ImageAnalyser() {
}

Mat& ImageAnalyser::getAnalysedImage()
{
	return analysedImg;
}

void ImageAnalyser::show(GUI& pGui) {
	gui = &pGui;
	if (showOrig) gui->addWindow(WINDOW_IA_ORIG);
	if (showInrange) gui->addWindow(WINDOW_IA_INRANGE);
	gui->addWindow(WINDOW_IA_CONTOURS);

	if (showOrig) setMouseCallback(WINDOW_IA_ORIG, mouseCallBackFuncOrig, &imageToAnalyse);
	if (showInrange) createTrackbar(trackbar_range, WINDOW_IA_INRANGE, &userInputThresholdRange,
			max_value, adjustParameters, this);
	setMouseCallback(WINDOW_IA_CONTOURS, mouseCallBackFuncContours, &analysedImg);

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

	int bestIdx = findBestCountour(contours, colorBlob.getMinSize(), colorBlob.getMaxSize());
	if (bestIdx == -1)	return Point2f();

	minEnclosingCircle(contours[bestIdx], center, radius);

	// draw
	frame.copyTo(analysedImg);
	for (unsigned i = 0; i < contours.size(); i++) {
		drawContours(analysedImg, contours, i, Scalar(128, 255, 255), 1);
	}
	circle(analysedImg, center, 10, Scalar(0, 255, 255), 4, 8, 0);
	drawContours(analysedImg, contours, bestIdx, Scalar(255, 128, 255), 3);
	gui->showImage(WINDOW_IA_CONTOURS, analysedImg);

	return center;
}

bool ImageAnalyser::detectByContours(Mat &frame, TrackedObject& trackedObject) {

	// store parameters for callback function
	pTrackedObject = &trackedObject;
	frame.copyTo(imageToAnalyse);
	if (showOrig) gui->showImage(WINDOW_IA_ORIG, imageToAnalyse);

	int leftBlobIdx = 0;
	int rightBlobIdx = 0;
	vector<vector<Point> > leftContours;
	vector<vector<Point> > rightContours;
	vector<TrackedColorBlob> colorBlobs = trackedObject.getColorBlobs();

	//
	// tracked object where we cannot distinguish colorblobs by color range, e.g. light by night
	//
	if (colorBlobs.size() == 1)
	{
		TrackedColorBlob colorBlob = colorBlobs[0];

		leftContours = findCountours(imageToAnalyse, colorBlob);
		rightContours = leftContours;
		if (leftContours.size() < 1) return false;

		if (leftContours.size() == 2 )
		{
			Mat1b mask(imageToAnalyse.rows, imageToAnalyse.cols, uchar(0));
			drawContours(mask, leftContours, 0, Scalar(255,255,255), CV_FILLED);
			Scalar average0 = mean(imageToAnalyse, mask);
			cout << "average0: " << average0 << endl;

			mask(imageToAnalyse.rows, imageToAnalyse.cols, uchar(0));
			drawContours(mask, leftContours, 1, Scalar(255,255,255), CV_FILLED);
			Scalar average1 = mean(imageToAnalyse, mask);
			cout << "average1: " << average1 << endl;

			// select red light by smaler average red component
			if (average0[2] < average1[2])
				leftBlobIdx = 1;
			else
				rightBlobIdx = 1;
		}
	}

	//
	// tracked Object with two colors
	//
	if (colorBlobs.size() == 2)
	{
		TrackedColorBlob colorBlob = colorBlobs[0];
		leftContours = findCountours(imageToAnalyse, colorBlob);
		leftBlobIdx = findBestCountour(leftContours, colorBlob.getMinSize(), colorBlob.getMaxSize());

		colorBlob = colorBlobs[1];
		rightContours = findCountours(imageToAnalyse, colorBlob);
		rightBlobIdx = findBestCountour(rightContours, colorBlob.getMinSize(), colorBlob.getMaxSize());
	}
	if (leftContours.size() < 1 || rightContours.size() < 1) return false;
	if (leftBlobIdx < 0 || rightBlobIdx < 0) return false;

	//
	// find centers of colorblobs
	//
	Point2f leftCenter;
	float radius;
	minEnclosingCircle(leftContours[leftBlobIdx], leftCenter, radius);

	Point2f rightCenter;
	minEnclosingCircle(rightContours[rightBlobIdx], rightCenter, radius);

	Point2f startPoint = (leftCenter + rightCenter) * .5; // this is also the center of both blobs

	if (norm(leftCenter - rightCenter) > 50)
	{
		// ERROR: distance too big (greater then 50 Pixels)
		return false;
	}

	//
	// calculate direction
	//
	Point2f v;
	v.x = rightCenter.x - leftCenter.x;
	v.y = rightCenter.y - leftCenter.y;
	// rotate vector 90 degrees
	float temp = v.y;
	v.y = -v.x;
	v.x = temp;
	Point2f endPoint = startPoint + v;

	//
	// update tracked objects position
	//
	trackedObject.setAktualPos(startPoint);
	trackedObject.setDirection(v);

	//
	// draw all found contours with small line,
	// draw yellow circle on center of best contours,
	// draw line indicating direction
	//
	frame.copyTo(analysedImg);
	for (unsigned i = 0; i < leftContours.size(); i++)
		drawContours(analysedImg, leftContours, i, Scalar(128, 255, 255), 1);
	for (unsigned i = 0; i < rightContours.size(); i++)
		drawContours(analysedImg, rightContours, i, Scalar(128, 255, 255), 1);

	circle(analysedImg, leftCenter, int(floor(radius)), Scalar(0, 0, 255), 4, 8, 0);
	circle(analysedImg, rightCenter, int(floor(radius)), Scalar(255, 255, 255), 4, 8, 0);

	line(analysedImg, startPoint, endPoint, Scalar(0, 255, 0), 1);
	gui->showImage(WINDOW_IA_CONTOURS, analysedImg);

	return true;
}

/**
 *
 */
vector<vector<Point> > ImageAnalyser::findCountours(Mat &frame,
		TrackedColorBlob& colorBlob) {

	cout << "ImageAnalyser::findCountours for colorBlob '" << colorBlob.getName() << " ";

	// either set threshold range to user input or to the range given by colorBlob
	OpenCVUtils utils;
	int range = colorBlob.getColorRange();

	// there is a range defined. use range to calulate upper and lower bound.
	if (userInputThresholdRange != 0) {
		range = userInputThresholdRange;
	}

	// see http://www.instructables.com/id/How-to-Track-your-Robot-with-OpenCV/step17/OpenCV-Selecting-Your-Color/
	Scalar treshLow = utils.gimpValue2OpenCV(colorBlob.getGimpColorLow(), -range);
	Scalar treshHi = utils.gimpValue2OpenCV(colorBlob.getGimpColorHigh(), range);

	//
	// prepare image for finding contours
	//
	Mat imgHSV;
	Mat imgThreshed;
	cvtColor(frame, imgHSV, CV_BGR2HSV); // change to HSV color space
	inRange(imgHSV, treshLow, treshHi, imgThreshed); // threshold
	if (showInrange) gui->showImage(WINDOW_IA_INRANGE, imgThreshed);

	//
	// find contours
	//
	vector<vector<Point> > contours0;
	vector<Vec4i> hierarchy;
	findContours(imgThreshed, contours0, hierarchy, RETR_TREE,
			CHAIN_APPROX_SIMPLE);

	if (contours0.size() == 0)
		cout << "Could not find any contour for colorBlob '" << colorBlob.getName() << "'. treshLow=" << treshLow << " treshHi=" << treshHi << endl;
	else
		cout << "Found " << contours0.size() << " contours" << endl;

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
		Size minSize, Size maxSize) {

	if (contour.size() == 1) return 0;

	cout <<  "ImageAnalyser::findBestCountour: ";
	vector<Point> bestContour;
	vector<vector<Point> > contours(contour.size());
	vector<Rect> boundRect(contours.size());
	vector<Point2f> center(contours.size());
	vector<float> radius(contours.size());
	int rightCountourIdx = -1;
	double minRadius = fmin(minSize.width, minSize.height) / 2.0;
	double maxRadius = fmax(maxSize.width, maxSize.height) / 2.0;
	for (unsigned int i = 0; i < contours.size(); i++) {
		approxPolyDP(Mat(contour[i]), contours[i], 3, true);
		boundRect[i] = boundingRect(Mat(contours[i]));
		minEnclosingCircle(contours[i], center[i], radius[i]);
		cout <<  "radius["<< i << "]=" << radius[i] << " ";
		if (radius[i] > minRadius && radius[i] < maxRadius) {
			rightCountourIdx = i;
			bestContour = contours[i];
		}
	}
	cout << " => best: " << rightCountourIdx << endl;

	if (rightCountourIdx == -1) {
		cout <<  "Could not find best contour for minSize: " <<  minSize << ", maxSize: " << maxSize << endl;
		return -1;
	}

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

	trackedObject.setAktualPos(Point_<int>(int(floor(moment10 / area)), int(floor(moment01 / area))));

	return true;

}

void ImageAnalyser::analyse(std::string imageName,
		TrackedObject& aTrackedObject) {

	//
	// Load an image
	Mat frame = imread(imageName, 1);

	//
	// mask out garden
	//
	Mat imageDest;
	Garden garden(1);
	imageDest = garden.maskOutGreen(frame);

	//
	// start analysing
	//
	analyse(imageDest, aTrackedObject);

//	waitKey(0);

	while (true)
	{
		char key = cv::waitKey(100);
		switch (key) {
		case 'o': // toggle visibility of orig window
			showOrig = !showOrig;
			destroyWindow(WINDOW_IA_ORIG);
			break;
		case 'i': // toggle visibility of inRange window
			showInrange = !showInrange;
			destroyWindow(WINDOW_IA_INRANGE);
			break;
		case 'w':
			lastRightClickPosition.y--;
			break;
		case 'y':
			lastRightClickPosition.y++;
			break;
		case 'a':
			lastRightClickPosition.x--;
			break;
		case 's':
			lastRightClickPosition.x++;
			break;
		case 'q':
			radius++;
			cout << "radius= " << radius;
			break;
		case 'e':
			radius--;
			cout << "radius= " << radius;
			break;
		case 'r':
			analyse(frame, aTrackedObject);
			break;

		default:
			continue;
		}
		// analyse(frame, aTrackedObject);

		Mat imgHSV;
		cvtColor(frame, imgHSV, CV_BGR2HSV);

		// find points within the circle
		Point c = lastRightClickPosition;
		int r = radius;
		Vec3b pMax = imgHSV.at<Vec3b>(c.y, c.x); //Vec3b - Array of 3 uchar numbers
		Vec3b pMin = imgHSV.at<Vec3b>(c.y, c.x); //Vec3b - Array of 3 uchar numbers

		for (int y = 0; y < frame.rows; ++y) {
		        for (int x = 0; x < frame.cols; ++x) {

		            // Check if this is an internal point
		            if ((x - c.x)*(x - c.x) + (y - c.y)*(y - c.y) <= (r*r)) {
		                //allpoints.push_back(Point(x,y));
		        		Vec3b p3 = imgHSV.at<Vec3b>(y, x); //Vec3b - Array of 3 uchar numbers
		        		for (int i=0; i<3;i++)
		        		{
							if (p3[i] > pMax[i]) pMax[i] = p3[i];
							if (p3[i] < pMin[i]) pMin[i] = p3[i];
		        		}
		            }
		        }
		    }

		Mat imgTmp;
		frame.copyTo(imgTmp);
		circle(imgTmp, lastRightClickPosition, radius, Scalar(255,255,255), 1, 8, 0);
		if (showOrig) gui->showImage(WINDOW_IA_ORIG, imgTmp);

		cout << "Color (pMin): " << (int) pMin[0] << ", " << (int) pMin[1] << ", "
				<< (int) pMin[2] << ")" << endl;
		cout << "Color (pMax): " << (int) pMax[0] << ", " << (int) pMax[1] << ", "
				<< (int) pMax[2] << ")" << endl;

	}
}

void ImageAnalyser::analyse(Mat& frame,
		TrackedObject& aTrackedObject) {
	frame.copyTo(imageToAnalyse);
	pTrackedObject = &aTrackedObject;
	if (showOrig) gui->showImage(WINDOW_IA_ORIG, imageToAnalyse);

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

void mouseCallBackFuncOrig(int event, int x, int y, int flags, void* userdata) {
	Mat* rgb = (Mat*) userdata;
	if (rgb->dims == 0) return;
	if (event == EVENT_LBUTTONDOWN) {
		// cout << "Left button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;

		Mat imgHSV;
		cvtColor(*rgb, imgHSV, CV_BGR2HSV);

		Vec3b p1 = (*rgb).at<Vec3b>(y, x); //Vec3b - Array of 3 uchar numbers
		Vec3b p2 = imgHSV.at<Vec3b>(y, x); //Vec3b - Array of 3 uchar numbers
//		Vec3b pMax = imgHSV.at<Vec3b>(y, x); //Vec3b - Array of 3 uchar numbers
//		Vec3b pMin = imgHSV.at<Vec3b>(y, x); //Vec3b - Array of 3 uchar numbers


		Mat imgTmp;
		rgb->copyTo(imgTmp);
		circle(imgTmp, Point(x,y), 10, Scalar(255,255,255), 1, 8, 0);
		// geht ned!!! if (showOrig) gui->showImage(WINDOW_IA_ORIG, imgTmp);

		//p[0] - H, p[1] - S, p[2] - V
		cout << endl << "Color (RGB): " << (int) p1[0] << ", " << (int) p1[1]
				<< ", " << (int) p1[2] << ")" << endl;
		cout << "Color (HSV): " << (int) p2[0] << ", " << (int) p2[1] << ", "
				<< (int) p2[2] << ")" << endl;

	} else if (event == EVENT_RBUTTONDOWN) {
		lastRightClickPosition = Point(x,y);
		cout << "Right button of the mouse is clicked - position (" << x << ", "
				<< y << ")" << endl;
	} else if (event == EVENT_MBUTTONDOWN) {
		cout << "Middle button of the mouse is clicked - position (" << x
				<< ", " << y << ")" << endl;
	} else if (event == EVENT_MOUSEMOVE) {
		// cout << "Mouse move over the window - position (" << x << ", " << y << ")" << endl;
	}
}

void mouseCallBackFuncContours(int event, int x, int y, int flags, void* userdata) {
	Mat* rgb = (Mat*) userdata;
	if (rgb->dims == 0) return;
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

