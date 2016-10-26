/*
 * VideoCamera.cpp
 *
 *  Created on: Oct 14, 2016
 *      Author: edi
 */

#include "VideoCamera.h"

#include "OpenCVUtils.h"
#include <stdio.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>


using namespace std;
using namespace cv;

VideoCamera::VideoCamera()  {

	OpenCVUtils util;

	//
	// open camera
	//
//	cap.open(0);
	//cap.open("http://iris.not.iac.es/axis-cgi/mjpg/video.cgi?resolution=320x240");
//	cap.open("http://88.53.197.250/axis-cgi/mjpg/video.cgi?resolution=320x240");

	string filename =
			"/home/edi/workspace/robosheep/resources/M20120703_200959.avi";
	cap.open(filename);

	if (!cap.isOpened()) {
		if (util.isFileReadable(filename.c_str())) {
			printf(
					"\n\n!Error! - File is readable but not vor cap.\n probably OpenCV does not support ffmpeg\n hit a key\n");
		} else {
			printf(
					"\n\n!Error! - Faild to connect to web camera\n hit a key\n");
		}

		// cv::waitKey();
		// return 0;
	}

	printf("\n\nVideoCapture properties:\n");
	printf("CV_CAP_PROP_FRAME_WIDTH properties:  %f\n",
			cap.get(CV_CAP_PROP_FRAME_WIDTH));
	printf("CV_CAP_PROP_FRAME_HEIGHT properties: %f\n",
			cap.get(CV_CAP_PROP_FRAME_HEIGHT));
	printf("CV_CAP_PROP_FPS properties:          %f\n",
			cap.get(CV_CAP_PROP_FPS));
	printf("CV_CAP_PROP_FORMAT properties:       %f\n",
			cap.get(CV_CAP_PROP_FORMAT));
	printf("CV_CAP_PROP_MODE properties:         %f\n",
			cap.get(CV_CAP_PROP_MODE));
	printf("CV_CAP_PROP_BRIGHTNESS properties:   %f\n",
			cap.get(CV_CAP_PROP_BRIGHTNESS));
	printf("CV_CAP_PROP_CONTRAST properties:     %f\n",
			cap.get(CV_CAP_PROP_CONTRAST));
	printf("CV_CAP_PROP_SATURATION properties:   %f\n",
			cap.get(CV_CAP_PROP_SATURATION));
	printf("CV_CAP_PROP_HUE properties:          %f\n",
			cap.get(CV_CAP_PROP_HUE));
	printf("CV_CAP_PROP_GAIN properties:         %f\n",
			cap.get(CV_CAP_PROP_GAIN));
	printf("CV_CAP_PROP_EXPOSURE properties:     %f\n",
			cap.get(CV_CAP_PROP_EXPOSURE));
	printf("CV_CAP_PROP_CONVERT_RGB properties:  %f\n",
			cap.get(CV_CAP_PROP_CONVERT_RGB));
	printf("\n\n");


	namedWindow("contours", 1);
	moveWindow("contours", 100,100);

	namedWindow("imgThreshed", 1);
	moveWindow("imgThreshed", 700,100);


}

VideoCamera::~VideoCamera() {
	// TODO Auto-generated destructor stub
}

bool VideoCamera::read(Mat& frame) {
	return cap.read(frame);
}

bool VideoCamera::read(Mat& frame, int frameDelay) {
	if (!cap.read(frame))
			return false;

	char key = waitKey(frameDelay);
	switch (key) {
	case 27:
		return false;
		break;
	}
	return true;
}


bool VideoCamera::detectObjectPosition(TrackedObject& trackedObject)
{
	//
	// read image from video
	//
	Mat frame;
	if (!cap.read(frame)) {
		return false;
	}

	return detectObjectPosition(trackedObject);
}

bool VideoCamera::detectObjectPosition(Mat& frame, TrackedObject& trackedObject)
{

	// see http://www.instructables.com/id/How-to-Track-your-Robot-with-OpenCV/step17/OpenCV-Selecting-Your-Color/
	Scalar treshColorLow = gimpValue2OpenCV(trackedObject.getGimpColor(), -trackedObject.getColorRange());
	Scalar treshColorHi = gimpValue2OpenCV(trackedObject.getGimpColor(), trackedObject.getColorRange());

	//
	// prepare image for finding contours
	//
	Mat imgHSV;
	Mat imgThreshed;
	cvtColor(frame, imgHSV, CV_BGR2HSV); // change to HSV color space
	inRange(imgHSV, treshColorLow, treshColorHi, imgThreshed); // treshhold
	imshow("imgThreshed", imgThreshed);

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
	Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
	for (uint i = 0; i < contours.size(); i++) {
		drawContours(cnt_img, contours, i, Scalar(128, 255, 255), 1, CV_AA,
				hierarchy, std::abs(_levels));
//		circle(cnt_img, center[i], (int) radius[i], color, 2, 8, 0);
	}

	if (rightCountourIdx == -1) {
		printf("Coulnd not find rightCountourIdx\n");
		return true;
	}

	drawContours(cnt_img, contours, rightCountourIdx, Scalar(255, 128, 255), 3,
			CV_AA, hierarchy, std::abs(_levels));
	circle(cnt_img, center[rightCountourIdx], 10, color, 4, 8, 0);

	//
	// show contours image cnt_img
	//
	imshow("contours", cnt_img);

	//
	// update tracked objects position
	//
	trackedObject.setAktualPos(center[rightCountourIdx]);

	return true;
}

/**
 * HSV value range in Gimp is H = 0-360, S = 0-100, V = 0-100.
 * In                 OpenCV, H = 0-180, S = 0-255, V = 0-255.
 */
Scalar VideoCamera::gimpValue2OpenCV(Scalar gimpValue, int range) {
	Scalar opencvValue(gimpValue[0] * 180 / 360 + range,
			gimpValue[1] * 255 / 100 + range, gimpValue[2] * 255 / 100 + range);
	return opencvValue;
}
