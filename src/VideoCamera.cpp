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
using namespace robosheep;


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



