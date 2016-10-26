/*
 * VideoCamera.h
 *
 *  Created on: Oct 14, 2016
 *      Author: edi
 */

#ifndef SRC_VIDEOCAMERA_H_
#define SRC_VIDEOCAMERA_H_

#include <opencv2/highgui/highgui.hpp>

#include "OpenCVUtils.h"
#include "virtualSheep.h"


using namespace cv;

class VideoCamera {
	VideoCapture cap;

	// the detected sheep position
	Point_<int> lastRoboPos;
	Point_<int> roboPos;

	Scalar gimpValue;
	Scalar treshColorLow;
	Scalar treshColorHi;


public:
	VideoCamera();
	virtual ~VideoCamera();
	bool read(Mat& frame);
	Point_<int>  detectSheepPosition(virtualSheep sheep);
	Point_<int>  getLastRoboPos();
	Scalar gimpValue2OpenCV(Scalar gimpValue, int range);
};

#endif /* SRC_VIDEOCAMERA_H_ */
