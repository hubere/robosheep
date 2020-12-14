/*
 * A Garden object represents the ground which is to be mowed. At this point
 * in time there are 2 Gardens known, my back yard and my cellar.
 *
 *  Created on: Sep 19, 2012
 *      Author: Edwin Huber
 */

#ifndef Garden_H_
#define Garden_H_


#include "presentation/GUI.h"

#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc.hpp>


using namespace std;
using namespace cv;

namespace robosheep {

enum
{
	GARDEN,
	CELLAR
};

class Garden {
	GUI* gui;
	vector<Point> greenContour;
	vector<Point> route;
	Mat gardenImage;
	size_t nextRoutePointIdx;

public:
	Garden(int gardenIdx);
	virtual ~Garden();

	void show(GUI& gui);
	void setImage(Mat& image);
	Mat maskOutGreen(Mat& frame);
	Mat showGreen(Mat &image);
	Mat showRoute(Mat &image);

	void startRoute();
	Point getNextRoutePoint();
	vector<Point> &getGreenContour();
	vector<Point> &getRoute();
	size_t getRouteSize();

};

}  // namespace robosheep

#endif /* Garden_H_ */
