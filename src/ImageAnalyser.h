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

static const string ALGORITHM_DETECTBYMOMENTS = "ALGORITHM_DETECTBYMOMENTS";
static const string ALGORITHM_DETECTBYCONTOURS = "ALGORITHM_DETECTBYCONTOURS";

/**
 * Analyse an image in order to find position of a trackedObject.
 *
 */
class ImageAnalyser {

	TrackedObject* pTrackedObject;
	Mat* pFrame;
	string algorithm;
	Mat analysedImg;

public:
	ImageAnalyser();
	virtual ~ImageAnalyser();
	void show(GUI& gui);
	void setFrame(Mat &frame);
	Point2f detectObjectPosition(Mat &frame, TrackedColorBlob& colorBlob);
	bool detectObjectPosition(Mat& frame, TrackedObject& trackedObject);
	bool detectObjectPosition();
	void analyse(string imageName, TrackedObject& aTrackedObject);
	void analyse(Mat& frame, TrackedObject& aTrackedObject);

private:
	vector<vector<Point> > findCountours(Mat &frame,
			TrackedColorBlob& colorBlob);
	int findBestCountourWithMaximumArea(vector<vector<Point> > &contour);
	int findBestCountour(vector<vector<Point> > &contour, Size objSize);
	bool detectByContours(Mat &frame, TrackedObject& trackedObject);
	bool detectByMoments(Mat &frame, TrackedObject& trackedObject);
};

#endif /* ImageAnalyser_H_ */
