/*
 * ImageAnalyser.h
 *
 *  Created on: Sep 19, 2012
 *      Author: ubuntu
 */

#ifndef ImageAnalyser_H_
#define ImageAnalyser_H_

#include <opencv2/core/core.hpp>

#include "GUI.h"
#include "TrackedObject.h"


using namespace std;
using namespace cv;
using namespace robosheep;

/**
 * Analyse an image in order to find position of a trackedObject.
 *
 */
class ImageAnalyser {

	TrackedObject* pTrackedObject;
	Mat* pFrame;

public:
	ImageAnalyser();
	virtual ~ImageAnalyser();
	void show(GUI& gui);
	bool detectObjectPosition(Mat& frame, TrackedObject& trackedObject);
	bool detectObjectPosition();
	void analyse(std::string imageName, TrackedObject& aTrackedObject);

private:

};

#endif /* ImageAnalyser_H_ */
