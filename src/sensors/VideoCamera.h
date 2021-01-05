/*
 * The VideoCamera reads images from a cameraURL. 
 *
 *  Created on: Oct 14, 2016
 *      Author: Edwin Huber
 */

#ifndef SRC_SENSORS_VIDEOCAMERA_H_
#define SRC_SENSORS_VIDEOCAMERA_H_

#include "presentation/GUI.h"
#include "utilities/Stopwatch.h"

using namespace cv;

namespace robosheep {

class VideoCamera {
	GUI* gui;				// to display aquired frame and measurements
	VideoCapture cap;		// the capture device, i.e. video camera
	VideoWriter* writer;	// for writing a log-video
	Mat image;				// the most recent aquired image
	Stopwatch stopwatch;	// used for time measurement
	int fps;				// frames per second; used for performance measurement

public:
	VideoCamera();
	virtual ~VideoCamera();
	bool open(String& url);
	void test(String& url);
	void loadImage(String image);
	void show(GUI& gui);
	bool read(Mat& frame, int frameDelay);
	bool read(Mat& frame);
	bool takeSnapshot(Mat& frame);
	bool saveFrame();
	int  getFPS();
	void write(Mat& frame);

private:
	void probeUrls();

};

}  // namespace robosheep

#endif /* SRC_SENSORS_VIDEOCAMERA_H_ */
