/*
 * Garden.cpp
 *
 *  Created on: Sep 19, 2012
 *      Author: ubuntu
 */

#include "Garden.h"

#include <stdio.h>

Garden::Garden() {

	//
	// get contours for greens
	//

	// Create a sequence of points to make a contour:
	// green = new vector<Point>(6);
	green[0] = Point(104, 221);
	green[1] = Point(119, 403);
	green[2] = Point(345, 404);
	green[3] = Point(339, 323);
	green[4] = Point(498, 321);
	green[5] = Point(464, 228);

	// vector<vector<Point> > greenContours(1);
	greenContours[0] = green;

	//
	// get route
	//

	// vector<Point> route(10);
	route[0] = Point(129, 391);
	route[1] = Point(118, 231);
	route[2] = Point(134, 232);
	route[3] = Point(142, 394);
	route[4] = Point(159, 395);
	route[5] = Point(142, 232);
	route[6] = Point(157, 232);
	route[7] = Point(172, 395);
	route[8] = Point(188, 396);
	route[9] = Point(170, 232);

	routes[0] = route;

}

Garden::~Garden() {
	// TODO Auto-generated destructor stub
}

Point_<int> Garden::getRoutePoint(int routeIdx) {
	return route[routeIdx];
}

vector<vector<Point> > &Garden::getGreenContours() {
	return greenContours;
}
vector<vector<Point> > &Garden::getRoutes() {
	return routes;
}

int Garden::getRouteSize(){
	return route.size();
}


