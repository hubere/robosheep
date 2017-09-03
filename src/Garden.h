/*
 * Garden.h
 *
 *  Created on: Sep 19, 2012
 *      Author: ubuntu
 */

#ifndef Garden_H_
#define Garden_H_


#include "GUI.h"

#include <opencv2/core/core.hpp>
#include <vector>


using namespace std;
using namespace cv;

enum
{
	GARDEN,
	CELLAR
};

class Garden {
	vector<Point> greenContour;
	vector<Point> route;
	Mat gardenImage;

public:
	Garden(int gardenIdx);
	virtual ~Garden();

	void show(GUI& gui);
	void setImage(Mat& image);
	Mat maskOutGreen(Mat& frame);
	Mat showGreen(Mat &image);
	Mat showRoute(Mat &image);


	Point_<int> getRoutePoint(int id);
	vector<Point> &getGreenContour();
	vector<Point> &getRoute();
	int getRouteSize();
};

#endif /* Garden_H_ */
