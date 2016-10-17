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


using namespace std;
using namespace cv;


VideoCamera::VideoCamera() {

	OpenCVUtils util;

	//
	// open camera
	//
//	cap.open(0);
	//cap.open("http://iris.not.iac.es/axis-cgi/mjpg/video.cgi?resolution=320x240");
//	cap.open("http://88.53.197.250/axis-cgi/mjpg/video.cgi?resolution=320x240");
	cap.open("/home/edi/workspace/robosheep/resources/M20120703_200959.avi");

	string filename = "M20120703_200959.avi";
//
//	CvCapture* capture = cvCaptureFromAVI("M20120703_200959.avi");
//
//	cap.open(filename);

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
	printf("CV_CAP_PROP_FRAME_WIDTH properties:  %d\n",
			cap.get(CV_CAP_PROP_FRAME_WIDTH));
	printf("CV_CAP_PROP_FRAME_HEIGHT properties: %d\n",
			cap.get(CV_CAP_PROP_FRAME_HEIGHT));
	printf("CV_CAP_PROP_FPS properties:          %d\n",
			cap.get(CV_CAP_PROP_FPS));
	printf("CV_CAP_PROP_FORMAT properties:       %d\n",
			cap.get(CV_CAP_PROP_FORMAT));
	printf("CV_CAP_PROP_MODE properties:         %d\n",
			cap.get(CV_CAP_PROP_MODE));
	printf("CV_CAP_PROP_BRIGHTNESS properties:   %d\n",
			cap.get(CV_CAP_PROP_BRIGHTNESS));
	printf("CV_CAP_PROP_CONTRAST properties:     %d\n",
			cap.get(CV_CAP_PROP_CONTRAST));
	printf("CV_CAP_PROP_SATURATION properties:   %d\n",
			cap.get(CV_CAP_PROP_SATURATION));
	printf("CV_CAP_PROP_HUE properties:          %d\n",
			cap.get(CV_CAP_PROP_HUE));
	printf("CV_CAP_PROP_GAIN properties:         %d\n",
			cap.get(CV_CAP_PROP_GAIN));
	printf("CV_CAP_PROP_EXPOSURE properties:     %d\n",
			cap.get(CV_CAP_PROP_EXPOSURE));
	printf("CV_CAP_PROP_CONVERT_RGB properties:  %d\n",
			cap.get(CV_CAP_PROP_CONVERT_RGB));
	printf("\n\n");

}

VideoCamera::~VideoCamera() {
	// TODO Auto-generated destructor stub
}

bool VideoCamera::read(cv::Mat frame)
{
	return cap.read(frame);
}

Point_<int>  VideoCamera::getLastRoboPos(){
	return lastRoboPos;
}

Point_<int>  VideoCamera::detectSheepPosition(virtualSheep sheep)
{
	cv::Mat frame;

	cap.read(frame);

	/// Convert image to gray and blur it
	Mat src_gray;
	cvtColor(frame, src_gray, CV_BGR2GRAY);
	blur(src_gray, src_gray, Size(3, 3));

	/// Detect edges using Threshold
	Mat threshold_output;
	int thresh = 200;
	int max_thresh = 255;
	threshold(src_gray, threshold_output, thresh, 255, THRESH_BINARY);

	Mat imgHSV;
	Mat imgThreshed;
	Scalar treshColorLow(0, 200, 200);
	Scalar treshColorHi(40, 255, 255);

	// change to HSV color space
	cvtColor(frame, imgHSV, CV_BGR2HSV);

	// treshhold
	inRange(imgHSV, treshColorLow, treshColorHi, imgThreshed);

	namedWindow("contours", 1);

	vector<vector<Point> > contours0;
	vector<Vec4i> hierarchy;
	findContours(imgThreshed, contours0, hierarchy, RETR_TREE,
			CHAIN_APPROX_SIMPLE);
	if (contours0.size() != 1) {
		printf(
				"ERROR! found more or less than one contour! contours count: %i\n",
				contours0.size());
	}

	/// Approximate contours to polygons + get bounding rects and circles
	vector<vector<Point> > contours(contours0.size());
	vector<Rect> boundRect(contours.size());
	vector<Point2f> center(contours.size());
	vector<float> radius(contours.size());
	int rightCountourIdx = -1;
	Size sheepSize = sheep.getSize();
	float minRadius = fmin(sheepSize.width, sheepSize.height) / 2.0;
	float maxRadius = fmax(sheepSize.width, sheepSize.height) * 2.0;
	for (int i = 0; i < contours.size(); i++) {
		approxPolyDP(Mat(contours0[i]), contours[i], 3, true);
		boundRect[i] = boundingRect(Mat(contours[i]));
		minEnclosingCircle(contours[i], center[i], radius[i]);
		if (radius[i] > minRadius and radius[i] < maxRadius)
			rightCountourIdx = i;
	}

	Mat cnt_img;
	frame.copyTo(cnt_img);

	RNG rng(12345);
	int _levels = 3;
	Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255),
			rng.uniform(0, 255));
	for (uint i = 0; i < contours.size(); i++) {
		drawContours(cnt_img, contours, i, Scalar(128, 255, 255), 3, CV_AA,
				hierarchy, std::abs(_levels));
		circle(cnt_img, center[i], (int) radius[i], color, 2, 8, 0);
	}

	if (rightCountourIdx == -1) {
		return Point(0, 0);
	}

	circle(cnt_img, center[rightCountourIdx], 10, color, 4, 8, 0);

	imshow("contours", cnt_img);

	lastRoboPos = roboPos;
	roboPos = center[rightCountourIdx];

	return roboPos;
}
