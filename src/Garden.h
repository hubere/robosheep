/*
 * Garden.h
 *
 *  Created on: Sep 19, 2012
 *      Author: ubuntu
 */

#ifndef Garden_H_
#define Garden_H_

#include <opencv2/core/core.hpp>
#include <vector>


using namespace std;
using namespace cv;

class Garden {
	vector<Point> green;
	vector<vector<Point> > greenContours;
	vector<Point> route;
	vector<vector<Point> > routes;

public:
	Garden();
	virtual ~Garden();
	Point_<int> getRoutePoint(int id);
	vector<vector<Point> > &getGreenContours();
	vector<vector<Point> > &getRoutes();
	int getRouteSize();
};

#endif /* Garden_H_ */
