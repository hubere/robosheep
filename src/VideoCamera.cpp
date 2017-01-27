/*
 * VideoCamera.cpp
 *
 *  Created on: Oct 14, 2016
 *      Author: edi
 */

#include "VideoCamera.h"

#include "OpenCVUtils.h"
#include "GUI.h"
#include <stdio.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

static const string WINDOW_VIDEO = "Camera";

VideoCamera::VideoCamera() {

	OpenCVUtils util;

	//
	// open camera
	//
	string url;

	// --- URLs ---
	// url = "http://iris.not.iac.es/axis-cgi/mjpg/video.cgi?resolution=320x240";
	//	url = "http://88.53.197.250/axis-cgi/mjpg/video.cgi?resolution=320x240";
	// url = "http://192.168.0.115/image/jpeg.cgi";
	url = "http://admin:hubercek@192.168.0.115/video.cgi";
	//url = "http://192.168.0.115/video.cgi";

	// --- files ---
	//	 url =		"/home/edi/workspace/robosheep/resources/M20120703_200959.avi";

	printf("\nTry to open '0' ...");
	cap.open(0);
	if (!cap.isOpened()) {
		printf("failed\n");

		url = "http://192.168.0.115/video.cgi";
		printf("\nTry to open '%1$s' ...", url.c_str());
		cap.open("http://192.168.0.115/video.cgi");
		if (!cap.isOpened()) {
			printf("failed\n");

			url = "http://admin:hubercek@192.168.0.115/video.cgi";
			printf("\nTry to open '%1$s' ...", url.c_str());
			cap.open("http://admin:hubercek@192.168.0.115/video.cgi");
			if (!cap.isOpened()) {
				printf("failed\n");

				url = "http://192.168.0.115/video.cgi?x.mjpg";
				printf("\nTry to open '%1$s' ...", url.c_str());
				cap.open(url.c_str());
				if (!cap.isOpened()) {
					printf("failed\n");
					return;
				}
			}
		}
	}
	printf("succeeded\n");

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

}

VideoCamera::~VideoCamera() {
	// TODO Auto-generated destructor stub
}

void VideoCamera::show(GUI& gui) {
	gui.addWindow(WINDOW_VIDEO);
}

bool VideoCamera::read(Mat& frame) {
	bool result = cap.read(frame);
	imshow(WINDOW_VIDEO, frame);
	return result;
}

bool VideoCamera::read(Mat& frame, int frameDelay) {
	if (!read(frame))
		return false;

	char key = waitKey(frameDelay);
	switch (key) {
	case 27:
		return false;
		break;
	}
	return true;
}

bool VideoCamera::takeSnapshot(Mat& frame) {
	bool result = cap.read(frame);
	return result;
}

