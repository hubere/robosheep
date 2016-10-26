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
#include "TrackedObject.h"


using namespace cv;

class VideoCamera {
	VideoCapture cap;

public:
	VideoCamera();
	virtual ~VideoCamera();
	bool read(Mat& frame, int frameDelay);
	bool read(Mat& frame);
	bool detectObjectPosition(TrackedObject& trackedObject);
	bool detectObjectPosition(Mat& frame, TrackedObject& trackedObject);
	Scalar gimpValue2OpenCV(Scalar gimpValue, int range);
};

#endif /* SRC_VIDEOCAMERA_H_ */
