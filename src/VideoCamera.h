/*
 * VideoCamera.h
 *
 *  Created on: Oct 14, 2016
 *      Author: edi
 */

#ifndef SRC_VIDEOCAMERA_H_
#define SRC_VIDEOCAMERA_H_

#include <opencv2/highgui/highgui.hpp>

#include "GUI.h"
#include "OpenCVUtils.h"
#include "TrackedObject.h"


using namespace cv;

class VideoCamera {
	GUI* gui;
	VideoCapture cap;
	Mat image;

public:
	VideoCamera();
	virtual ~VideoCamera();
	void test(String& url);
	bool open(String& url);
	void loadImage(String image);
	void show(GUI& gui);
	bool read(Mat& frame, int frameDelay);
	bool read(Mat& frame);
	bool takeSnapshot(Mat& frame);
	bool saveFrame();

private:
	void probeUrls();


};

#endif /* SRC_VIDEOCAMERA_H_ */
