/*
 * OpenCVUtils.cpp
 *
 *  Created on: Oct 2, 2012
 *      Author: ubuntu
 */

#include <stdio.h>
#include "OpenCVUtils.h"

#include <iostream>
#include <fstream>

using namespace std;

namespace robosheep {

#define PI 3.14159265
#define GetCurrentDir getcwd

OpenCVUtils::OpenCVUtils() {
	// TODO Auto-generated constructor stub
}

OpenCVUtils::~OpenCVUtils() {
	// TODO Auto-generated destructor stub
}

void OpenCVUtils::printWorkingDirectory() {
	//
	// print working directory
	//
	char *path = NULL;
//	path = getcwd(); // or _getcwd
	if (path != NULL
		)
		printf("Working directory: %s\n", path);
}

/*
 * CV_CAP_PROP_POS_MSEC Current position of the video file in milliseconds or video capture timestamp.
 * CV_CAP_PROP_POS_FRAMES 0-based index of the frame to be decoded/captured next.
 * CV_CAP_PROP_POS_AVI_RATIO Relative position of the video file: 0 - start of the film, 1 - end of the film.
 * CV_CAP_PROP_FRAME_WIDTH Width of the frames in the video stream.
 * CV_CAP_PROP_FRAME_HEIGHT Height of the frames in the video stream.
 * CV_CAP_PROP_FPS Frame rate.
 * CV_CAP_PROP_FOURCC 4-character code of codec.
 * CV_CAP_PROP_FRAME_COUNT Number of frames in the video file.
 * CV_CAP_PROP_FORMAT Format of the Mat objects returned by retrieve() .
 * CV_CAP_PROP_MODE Backend-specific value indicating the current capture mode.
 * CV_CAP_PROP_BRIGHTNESS Brightness of the image (only for cameras).
 * CV_CAP_PROP_CONTRAST Contrast of the image (only for cameras).
 * CV_CAP_PROP_SATURATION Saturation of the image (only for cameras).
 * CV_CAP_PROP_HUE Hue of the image (only for cameras).
 * CV_CAP_PROP_GAIN Gain of the image (only for cameras).
 * CV_CAP_PROP_EXPOSURE Exposure (only for cameras).
 * CV_CAP_PROP_CONVERT_RGB Boolean flags indicating whether images should be converted to RGB.
 * CV_CAP_PROP_WHITE_BALANCE Currently not supported
 * CV_CAP_PROP_RECTIFICATION Rectification flag for stereo cameras (note: only supported by DC1394 v 2.x backend currently)
 */
VideoCapture OpenCVUtils::openVideoCapture() {
	cv::VideoCapture cap;

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
		if (isFileReadable(filename.c_str())) {
			printf(
					"\n\n!Error! - File is readable but not vor cap.\n probably OpenCV does not support ffmpeg\n hit a key\n");
		} else {
			printf(
					"\n\n!Error! - Faild to connect to web camera\n hit a key\n");
		}

		// cv::waitKey();
		return cap;
	}

	printf("\n\nVideoCapture properties:\n");
	printf("CV_CAP_PROP_FRAME_WIDTH properties:  %f\n",			cap.get(cv::VideoCaptureProperties::CAP_PROP_FRAME_WIDTH));
	printf("CV_CAP_PROP_FRAME_HEIGHT properties: %f\n",			cap.get(cv::VideoCaptureProperties::CAP_PROP_FRAME_HEIGHT));
	printf("CV_CAP_PROP_FPS properties:          %f\n",			cap.get(cv::VideoCaptureProperties::CAP_PROP_FPS));
	printf("CV_CAP_PROP_FORMAT properties:       %f\n",			cap.get(cv::VideoCaptureProperties::CAP_PROP_FORMAT));
	printf("CV_CAP_PROP_MODE properties:         %f\n",			cap.get(cv::VideoCaptureProperties::CAP_PROP_MODE));
	printf("CV_CAP_PROP_BRIGHTNESS properties:   %f\n",			cap.get(cv::VideoCaptureProperties::CAP_PROP_BRIGHTNESS));
	printf("CV_CAP_PROP_CONTRAST properties:     %f\n",			cap.get(cv::VideoCaptureProperties::CAP_PROP_CONTRAST));
	printf("CV_CAP_PROP_SATURATION properties:   %f\n",			cap.get(cv::VideoCaptureProperties::CAP_PROP_SATURATION));
	printf("CV_CAP_PROP_HUE properties:          %f\n",			cap.get(cv::VideoCaptureProperties::CAP_PROP_HUE));
	printf("CV_CAP_PROP_GAIN properties:         %f\n",			cap.get(cv::VideoCaptureProperties::CAP_PROP_GAIN));
	printf("CV_CAP_PROP_EXPOSURE properties:     %f\n",			cap.get(cv::VideoCaptureProperties::CAP_PROP_EXPOSURE));
	printf("CV_CAP_PROP_CONVERT_RGB properties:  %f\n",			cap.get(cv::VideoCaptureProperties::CAP_PROP_CONVERT_RGB));
	printf("\n\n");

	return cap;
}

/**
 * Berechung des Kurswinkels
 * nach http://de.wikipedia.org/wiki/Richtungswinkel;
 * ACHTUNG! dort wird die x-Achse nach Norden ausgerichtet und die Drehrichtung ist rechtsdrehend!)
 */
double OpenCVUtils::getKurswinkel(Point2f Aufpunkt, Point2f Zielpunkt) {
	return -atan2(Zielpunkt.y - Aufpunkt.y, Zielpunkt.x - Aufpunkt.x);
}

int OpenCVUtils::getKurswinkelDegree(Point2f Aufpunkt, Point2f Zielpunkt) {
	int t = int(floor(getKurswinkel(Aufpunkt, Zielpunkt) / (PI / 180)));
	if (t < 0)
		t += 360;
	return t;
}

void OpenCVUtils::testKurswinkel() {
	Point center(100, 100);

	Point p000(150, 100);
	Point p090(100, 50);
	Point p180(50, 100);
	Point p270(100, 150);

	Point p045(150, 50);
	Point p135(50, 50);
	Point p225(50, 150);
	Point p315(150, 150);

	int ti = getKurswinkelDegree(center, p000);
	printf(" getKurswinkelDegree(center, p000)=%d\n", ti);

	ti = getKurswinkelDegree(center, p090);
	printf(" getKurswinkelDegree(center, p090)=%d\n", ti);

	ti = getKurswinkelDegree(center, p180);
	printf(" getKurswinkelDegree(center, p180)=%d\n", ti);

	ti = getKurswinkelDegree(center, p270);
	printf(" getKurswinkelDegree(center, p270)=%d\n", ti);

	ti = getKurswinkelDegree(center, p045);
	printf(" getKurswinkelDegree(center, p045)=%d\n", ti);

	ti = getKurswinkelDegree(center, p135);
	printf(" getKurswinkelDegree(center, p135)=%d\n", ti);

	ti = getKurswinkelDegree(center, p225);
	printf(" getKurswinkelDegree(center, p225)=%d\n", ti);

	ti = getKurswinkelDegree(center, p315);
	printf(" getKurswinkelDegree(center, p315)=%d\n", ti);

}

bool OpenCVUtils::isFileReadable(const char* filename) {
	// open a file in read mode.
	char data[100];

	ifstream infile;
	infile.open(filename);

	cout << "Reading from the file '" << filename << "'" << endl;
	infile >> data;

	// write the data at the screen.
	cout << data << endl;

	// close the opened file.
	infile.close();

	return true;
}

/**
 * HSV value range in Gimp is H = 0-360, S = 0-100, V = 0-100.
 * In                 OpenCV, H = 0-180, S = 0-255, V = 0-255.
 */
Scalar OpenCVUtils::gimpValue2OpenCV(Scalar gimpValue, int range) {
	Scalar opencvValue(max(gimpValue[0] * 180 / 360 + range, 0.0),
			max(gimpValue[1] * 255 / 100 + range, 0.0), max(gimpValue[2] * 255 / 100 + range, 0.0));
	return opencvValue;
}

Scalar OpenCVUtils::openCV2gimpValue(Scalar value) {
	Scalar gimpValue(value[0] / 180 * 360,
			value[1] / 255 * 100, value[2] / 255 * 100);
	return gimpValue;
}

}  // namespace robosheep
