/*
 * Garden.cpp
 *
 *  Created on: Sep 19, 2012
 *      Author: ubuntu
 */

#include "Garden.h"

#include <stdio.h>

namespace robosheep {

static const string WINDOW_GARDEN = "Garden";


Garden::Garden(int gardenIdx) : greenContour(), route() {

	startRoute();

	switch (gardenIdx) {
	case GARDEN: // unser Garten
	{
		//
		// get contours for greens
		//
		greenContour.push_back(Point(16, 396));
		greenContour.push_back(Point(268, 118));
		greenContour.push_back(Point(500, 69));
		greenContour.push_back(Point(413, 474));
		greenContour.push_back(Point(83, 472));

		//
		// get route to be driven
		//
		route.push_back(Point(129, 391));
//		route.push_back(Point(218, 300));
//		route[2] = Point(134, 232);
//		route[3] = Point(142, 394);
//		route[4] = Point(159, 395);
//		route[5] = Point(142, 232);
//		route[6] = Point(157, 232);
//		route[7] = Point(172, 395);
//		route[8] = Point(188, 396);
//		route[9] = Point(170, 232);
		break;
	}
	case CELLAR: // unser Keller
	{
		//
		// get contours for greens
		//

//		// Create a sequence of points to make a contour:
//		// green = new vector<Point>(6);
//		green[0] = Point(100, 260);
//		green[1] = Point(300, 260);
//		green[2] = Point(340, 460);
//		green[3] = Point(100, 460);
//
//		// vector<vector<Point> > greenContours(1);
//		greenContours[0] = green;
//
//		//
//		// get route
//		//
//
//		// vector<Point> route(10);
//		route[0] = Point(200, 300);
////		route[1] = Point(118, 231);
////		route[2] = Point(134, 232);
////		route[3] = Point(142, 394);
////		route[4] = Point(159, 395);
////		route[5] = Point(142, 232);
////		route[6] = Point(157, 232);
////		route[7] = Point(172, 395);
////		route[8] = Point(188, 396);
////		route[9] = Point(170, 232);
//
//		routes[0] = route;
//
		break;
	}
	}
}

Garden::~Garden() {
}

void Garden::show(GUI& pGui) {
	gui = &pGui;
	gui->addWindow(WINDOW_GARDEN);
}

void Garden::setImage(Mat& frame){
	gardenImage = maskOutGreen(frame);
	gardenImage = showRoute(gardenImage);
	gui->showImage(WINDOW_GARDEN, gardenImage);
}

Mat Garden::maskOutGreen(Mat& frame){

	Mat destImage;
	Mat mask = Mat::zeros(frame.rows, frame.cols, CV_8UC1);
	fillConvexPoly(mask, greenContour, Scalar(1.0, 1.0, 1.0), 16, 0);
	// Cut out ROI and store it in imageDest
	frame.copyTo(destImage, mask);
	return destImage;
}


Mat Garden::showGreen(Mat &image) {
	polylines(image, greenContour, true, Scalar(0, 0, 255), 2, 8);
	return image;
}

Mat Garden::showRoute(Mat &image) {
	polylines(image, route, true, Scalar(0, 0, 255), 2, 8);
	return image;
}

void Garden::startRoute()
{
	nextRoutePointIdx = -1;
}

Point Garden::getNextRoutePoint()
{
	nextRoutePointIdx++;
	if (nextRoutePointIdx >= getRouteSize())
		return Point(-1, -1);
	return route[nextRoutePointIdx];
}

vector<Point> &Garden::getGreenContour() {
	return greenContour;
}
vector<Point> &Garden::getRoute() {
	return route;
}

size_t Garden::getRouteSize() {
	return route.size();
}

}  // namespace robosheep