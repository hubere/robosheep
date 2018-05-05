/*
 * The VideoCamera reads images from a cameraURL. 
 *
 *  Created on: Oct 14, 2016
 *      Author: Edwin Huber
 */

#ifndef SRC_VIDEOCAMERA_H_
#define SRC_VIDEOCAMERA_H_

#include "GUI.h"
#include "Stopwatch.h"

using namespace cv;

class VideoCamera {
	GUI* gui;
	VideoCapture cap;
	VideoWriter* writer;
	Mat image;
	Stopwatch stopwatch;
	int fpms;

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
	int  getFPMS();
	void write(Mat& frame);

private:
	void probeUrls();


};

#endif /* SRC_VIDEOCAMERA_H_ */
