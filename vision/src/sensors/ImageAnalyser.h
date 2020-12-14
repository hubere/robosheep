/*
 * Analyse an image in order to find position of a trackedObject.
 *
 *  Created on: Sep 19, 2012
 *      Author: Edwin Huber
 */

#ifndef ImageAnalyser_H_
#define ImageAnalyser_H_

#include <opencv2/core/core.hpp>

#include "../presentation/GUI.h"
#include "TrackedObject.h"
#include "TrackedColorBlob.h"

using namespace std;
using namespace cv;

namespace robosheep {

static const string ALGORITHM_DETECTBYMOMENTS = "ALGORITHM_DETECTBYMOMENTS";
static const string ALGORITHM_DETECTBYCONTOURS = "ALGORITHM_DETECTBYCONTOURS";

class ImageAnalyser {
	GUI* gui;						// to display analysis information
	TrackedObject* pTrackedObject;	// the object that is to be tracked
	Mat imageToAnalyse;				// the image to be analysed
	string algorithm;				// the algorithm that is to be used to detect the trackedObject
	Mat analysedImg;				// image with analysis information

public:
	ImageAnalyser();
	virtual ~ImageAnalyser();
	void show(GUI& gui);
	bool detectObjectPosition(Mat& frame, TrackedObject& trackedObject);
	bool detectObjectPosition();
	void analyse(string imageName, TrackedObject& aTrackedObject);
	Mat& getAnalysedImage();

private:
	int radius;
	bool showOrig;					// controls whether to show original image
	bool showInrange;				// controls whether to show inRange image

	vector<vector<Point> > findCountours(Mat &frame, TrackedColorBlob& colorBlob);
	int findBestCountourWithMaximumArea(vector<vector<Point> > &contour);
	int findBestCountour(vector<vector<Point> > &contour, Size minSize, Size maxSize);
	bool detectByContours(Mat &frame, TrackedObject& trackedObject);
	bool detectByMoments(Mat &frame, TrackedObject& trackedObject);
	void analyse(Mat& frame, TrackedObject& aTrackedObject);
};

}  // namespace robosheep
#endif /* ImageAnalyser_H_ */
