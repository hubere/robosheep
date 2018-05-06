/*
 * VideoCamera.cpp
 *
 *  Created on: Oct 14, 2016
 *      Author: Edwin Huber
 */

#include "curl/curl.h" // has to go before opencv headers!!

#include "VideoCamera.h"

#include <stdio.h>
#include <iostream>
#include <time.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "OpenCVUtils.h"
#include "GUI.h"

using namespace std;
using namespace cv;

namespace robosheep {

// ----------------------------------------------
// Constants
// ----------------------------------------------

static const string WINDOW_VIDEO = "Camera";
static int framesTaken = 0;

void mouseCallBackVideo(int event, int x, int y, int flags, void* userdata);
Mat curlImg(const char *img_url, int timeout = 10);


// ----------------------------------------------
// Class methods
// ----------------------------------------------

VideoCamera::VideoCamera() {
	gui = NULL;
	writer = NULL;
	fpms = 0;
	stopwatch.reset();
}

VideoCamera::~VideoCamera() {
	if (writer != NULL)
		delete writer;
}

/**
* open camera stream
*/
bool VideoCamera::open(String& url) {

	cout << endl << "VideoCamera::open(" << url << ") -> ";
	if (!cap.open(url)){
		cout << "FAILED!" << endl;
		// probeUrls(); <- for debugging purpose, enable probeUrls()
		cout << endl << "VideoCamera::open	ensure camera is switched on " << endl;
		cout         << "			and use fing to find IP of camera." << endl;
		cout         << "			adjust parameter --cameraURL to the ip-address." << endl;
		return false;
	}
	cout << "succeeded." << endl;
	return true;
}

void VideoCamera::loadImage(String imageName){
	image = imread(imageName, 1);
}

void VideoCamera::show(GUI& pGui) {
	gui = &pGui;
	gui->addWindow(WINDOW_VIDEO);
	setMouseCallback(WINDOW_VIDEO, mouseCallBackVideo, &image);
}

bool VideoCamera::read(Mat& frame) {
	bool result = true;
	if (!image.empty())
		frame = image;
	else
	{
		cap >> frame;
	}
	
	if (frame.empty())
		return false;

	int measuringTime = 10; // measure all seconds
	if (stopwatch.getElapsedTime() > measuringTime * 1000)
	{
		fpms = framesTaken * 1000 / measuringTime;
		framesTaken = 0;
		stopwatch.reset();
		if (gui !=NULL)
			gui->showImage(WINDOW_VIDEO, frame);
	}

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
		strftime(filename, sizeof(filename), "snapshot_%Y-%m-%d_%H-%M-%S.jpg", gmtime(&now));
	}

	imwrite( filename, image );
	cout << "VideoCamera::saveFrame:	saved image to " << filename << endl;

	return true;
}

void VideoCamera::write(Mat& frame)
{
	if (writer == NULL)
	{
		int frame_width = frame.cols;
		int frame_height = frame.rows;
		writer = new VideoWriter("out.avi", CV_FOURCC('M', 'J', 'P', 'G'), 1, Size(frame_width, frame_height), true);
	}
	writer->write(frame);
}

int VideoCamera::getFPMS()
{
	return fpms;
}

void VideoCamera::test(String& url) {
	open(url);
	Mat frame;

	cap >> frame;
	if (!frame.empty()) gui->showImage(WINDOW_VIDEO, frame);
	for (;;) {

		// capture buffered frame
		cap >> frame;

		// capture single image
		//frame = curlImg("http://robosheep:mower@192.168.1.119/Image.jpg");

		if (frame.empty())
			break;

		++framesTaken;
		int measuringTime = 10; // measure all seconds
		if (stopwatch.getElapsedTime() > measuringTime * 1000)
		{
			fpms = framesTaken * 1000 / measuringTime;
			framesTaken = 0;
			stopwatch.reset();
			cout << "fpms: " << fpms << endl;
		}

		gui->showImage(WINDOW_VIDEO, frame);
		char key = (char)waitKey(10); //delay N millis, usually long enough to display and capture input

		switch (key) {
		case 'q':
		case 'Q':
		case 27: //escape key
			return;
		default:
			break;
		}
	}
}


//-----------------------------------------------
// private methods
//-----------------------------------------------

void VideoCamera::probeUrls() {

	string url;

	// --- URLs ---
	// url = "http://iris.not.iac.es/axis-cgi/mjpg/video.cgi?resolution=320x240";
	//	url = "http://88.53.197.250/axis-cgi/mjpg/video.cgi?resolution=320x240";
	// url = "http://192.168.1.101/image/jpeg.cgi";
	// url = "http://admin:hubercek@192.168.1.101/video.cgi";
	//url = "http://192.168.1.101/video.cgi";
	// URL die in VLC geht: http://robosheep:mower@192.168.1.128/video.cgi?x.mjpg

	//	Auflösung 1280x720: rtsp://192.168.1.111:554/onvif1 
	//	Auflösung 320x180: rtsp://192.168.1.111:554/onvif2

	// --- files ---
	//	 url =		"/home/edi/workspace/robosheep/resources/M20120703_200959.avi";

	printf("\nTry to open '%s' ...", url.c_str());
	cap.open("http://192.168.1.101/video.cgi");
	if (!cap.isOpened()) {
		printf("failed\n");

		url = "http://robosheep:mower@192.168.1.113:80/video.cgi?x.mjpg";
		printf("\nTry to open '%s' ...", url.c_str());
		cap.open("http://robosheep:mower@192.168.1.113:80/video.cgi?x.mjpg");
		if (!cap.isOpened()) {
			printf("failed\n");

			url = "http://192.168.1.101/video.cgi?x.mjpg";
			printf("\nTry to open '%s' ...", url.c_str());
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


//-----------------------------------------------
// static methods
//-----------------------------------------------

//curl writefunction to be passed as a parameter
// we can't ever expect to get the whole image in one piece,
// every router / hub is entitled to fragment it into parts
// (like 1-8k at a time),
// so insert the part at the end of our stream.
size_t write_data(char *ptr, size_t size, size_t nmemb, void *userdata)
{
	vector<uchar> *stream = (vector<uchar>*)userdata;
	size_t count = size * nmemb;
	stream->insert(stream->end(), ptr, ptr + count);
	return count;
}

//function to retrieve the image as cv::Mat data type
cv::Mat curlImg(const char *img_url, int timeout)
{
	vector<uchar> stream;
	CURL *curl = curl_easy_init();
	curl_easy_setopt(curl, CURLOPT_URL, img_url); //the img url
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data); // pass the writefunction
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &stream); // pass the stream ptr to the writefunction
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout); // timeout if curl_easy hangs, 
	// CURLcode res = curl_easy_perform(curl); // start curl
	curl_easy_cleanup(curl); // cleanup
	return imdecode(stream, -1); // 'keep-as-is'
}

void mouseCallBackVideo(int event, int x, int y, int flags, void* userdata) {
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

}  // namespace robosheep


