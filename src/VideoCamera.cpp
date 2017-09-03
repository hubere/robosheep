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
#include <iostream>
#include <time.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

//
// static stuff
//
static const string WINDOW_VIDEO = "Camera";

void mouseCallBackFunc(int event, int x, int y, int flags, void* userdata);


//
// constuctor
//
VideoCamera::VideoCamera() {
}

//
// open camera stream
//
bool VideoCamera::open(String& url) {

	cout << endl << "VideoCamera::open(" << url << ") -> ";
	if (!cap.open(url)){
		cout << "FAILED!" << endl;
		probeUrls();
		return false;
	}
	cout << "succeeded." << endl;
	return true;
}

void VideoCamera::loadImage(String imageName){
	// Load an image
	image = imread(imageName, 1);
}

void VideoCamera::probeUrls() {

	string url;

	// --- URLs ---
	// url = "http://iris.not.iac.es/axis-cgi/mjpg/video.cgi?resolution=320x240";
	//	url = "http://88.53.197.250/axis-cgi/mjpg/video.cgi?resolution=320x240";
	// url = "http://192.168.1.101/image/jpeg.cgi";
	// url = "http://admin:hubercek@192.168.1.101/video.cgi";
	//url = "http://192.168.1.101/video.cgi";

//	Auflösung 1280x720: rtsp://192.168.1.111:554/onvif1 
//	Auflösung 320x180: rtsp://192.168.1.111:554/onvif2

	// --- files ---
	//	 url =		"/home/edi/workspace/robosheep/resources/M20120703_200959.avi";

	printf("\nTry to open '%1$s' ...", url.c_str());
	cap.open("http://192.168.1.101/video.cgi");
	if (!cap.isOpened()) {
		printf("failed\n");

		url = "http://admin:hubercek@192.168.1.101/video.cgi";
		printf("\nTry to open '%1$s' ...", url.c_str());
		cap.open("http://admin:hubercek@192.168.1.101/video.cgi");
		if (!cap.isOpened()) {
			printf("failed\n");

			url = "http://192.168.1.101/video.cgi?x.mjpg";
			printf("\nTry to open '%1$s' ...", url.c_str());
			cap.open(url.c_str());
			if (!cap.isOpened()) {
				printf("failed\n");

				printf("\nTry to open USB0 ...");
				cap.open(0 + CV_CAP_PVAPI);
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
}

void VideoCamera::show(GUI& gui) {
	gui.addWindow(WINDOW_VIDEO);
	setMouseCallback(WINDOW_VIDEO, mouseCallBackFunc, &image);
}

bool VideoCamera::read(Mat& frame) {
	bool result = true;
	if (image.cols > 0)
		frame = image;
	else
		result = cap.read(frame);
	if (result) imshow(WINDOW_VIDEO, frame);
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

bool VideoCamera::saveFrame() {
	Mat image;
	if (!cap.read(image)){
		cout << "VideoCamera::saveFrame:	Could not read image" << endl;
		return false;
	}

	time_t now;
	char filename[40];
	filename[0] = '\0';
	now = time(NULL);


	   if (now != -1)
	   {
		   strftime(filename, sizeof(filename), "snapshot_%Y-%m-%d_%H:%M:%S.jpg", gmtime(&now));
	   }


	imwrite( filename, image );
	cout << "VideoCamera::saveFrame:	saved image to " << filename << endl;


	return true;
}


void mouseCallBackFunc(int event, int x, int y, int flags, void* userdata) {
	Mat* rgb = (Mat*) userdata;
	if (rgb->dims == 0) return;

	//
	// on left click, show coordinates
	//
	if (event == EVENT_LBUTTONDOWN) {

		cout << "position (" << x << ", " << y << ")" << endl;
		cout << "greenContour.push_back(Point("<< x <<", "<< y << ")" << endl;


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


