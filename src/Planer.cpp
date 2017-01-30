/*
 * Planer.cpp
 *
 *  Created on: Nov 10, 2016
 *      Author: edi
 */

#include "Planer.h"
#include "OpenCVUtils.h"

#include <opencv2/imgproc/imgproc.hpp>

#include <stdio.h>
#include <math.h>

using namespace std;
using namespace cv;

static const string WINDOW_PLANER = "Planer";
const bool useDistance = false;
static double dist2aim = std::numeric_limits<double>::max();
int numberPositions = 10;

Planer::Planer() :
		aim(0, 0), proximity(5), speed(0), motorSpeed1(0), motorSpeed2(0) {
}

Planer::~Planer() {
}

void Planer::show(GUI& gui) {
	gui.addWindow(WINDOW_PLANER);
	createTrackbar(" trajectory length:", WINDOW_PLANER, &numberPositions, 100);
}

void Planer::show(Mat& frame) {
	Mat planned;
	frame.copyTo(planned);

	//
	// draw aim
	//
	circle(planned, aim, 5, cvScalar(0, 255, 255), 1);

	//
	// draw lastdetected position
	Point roboPos = *lastPositions.end();
	circle(planned, roboPos, 5, cvScalar(0, 255, 0), 1);

	//
	// draw last positions
	//
	int myradius=5;
	for (int i=0;i<(int)lastPositions.size();i++)
	    circle(planned,Point(lastPositions[i].x,lastPositions[i].y),myradius,CV_RGB(100,0,0),-1,8,0);

	//
	// draw direction indicator
	//
	Point P2;
	Vec4f fittedline;
	fitLine(lastPositions, fittedline, CV_DIST_L2, 0, 0.01, 0.01);

	double theMult = max(frame.cols, frame.rows);
	// calculate start point
	Point startPoint;
	startPoint.x = fittedline[2] - theMult * fittedline[0]; // x0
	startPoint.y = fittedline[3] - theMult * fittedline[1]; // y0
	// calculate end point
	Point endPoint;
	endPoint.x = fittedline[2] + theMult * fittedline[0];		    //x[1]
	endPoint.y = fittedline[3] + theMult * fittedline[1];		    //y[1]

	Rect r = cv::boundingRect(lastPositions);
	clipLine(r.size(), startPoint, endPoint);
	line(planned, startPoint, endPoint, cvScalar(255, 0, 255), 3);

	//	P2.x =  (int)round(roboPos.x + length * cos(rotate * CV_PI / 180.0));
//	P2.y =  (int)round(roboPos.y + length * sin(rotate * CV_PI / 180.0));

//	circle(planned, aim, 5, cvScalar(0, 255, 255), 3);

	// line(frame, roboPos, P2, cvScalar(255, 0, 255), 3);

	imshow(WINDOW_PLANER, planned);
}

void Planer::setAim(Point2f newAim) {
	aim = newAim;
	printf("\nPlaner::setAim(%i,%i)\n", (int) aim.x, (int) aim.y);
}

Point2f Planer::getAim() {
	return aim;
}

void Planer::setAktualPosition(Point aktPos) {
	if (lastPositions.empty()) {
		for (int i = 0; i < numberPositions; i++)
			lastPositions.push_back(aktPos);
	}

	// implement ring buffer
	lastPositions.push_back(aktPos);
	lastPositions.erase(lastPositions.begin());
}

int Planer::getMotorSpeed1() {
	return motorSpeed1;
}

int Planer::getMotorSpeed2() {
	return motorSpeed2;
}

int Planer::plan(Point2f lastPos, Point2f aktPos) {
	OpenCVUtils util;

	// 1. calc direction to head
	int tiDegree = util.getKurswinkelDegree(lastPos, aktPos); // Kurswinkel ist
	int tsDegree = util.getKurswinkelDegree(aktPos, aim); // Kurswinkel soll

	// 2. calc movement commands
	int rotate = (tsDegree - tiDegree);
	if (rotate > 180)
		rotate = rotate - 360;
	if (rotate < -180)
		rotate = 360 + rotate;

	// 3. calc motor speeds
	int dist = (int) sqrt(
			(lastPos.x - aktPos.x) * (lastPos.x - aktPos.x)
					+ (lastPos.y - aktPos.y) * (lastPos.y - aktPos.y));
	motorSpeed1 = dist + rotate / 10;
	motorSpeed2 = dist - rotate / 10;

	if (dist < 10) {
		// sheep is not moving! => do not rotate
		rotate = 0;
	}

	printf("\nPlaner::plan aim=(%d,%d)\n", (int) aim.x, (int) aim.y);
	printf("	ti=%d ts=%d => rotate:%d\n", tiDegree, tsDegree, rotate);
	printf("	dist=%d m1=%d m2=%d\n", dist, motorSpeed1, motorSpeed2);

	return rotate;
}

bool Planer::isRoutePointReached(Point2f pos) {
	// using proximity
	if (aim.x > pos.x - proximity and aim.x < pos.x + proximity
			and aim.y > pos.y - proximity and aim.y < pos.y + proximity) {
		// we are in proximity

		if (!useDistance) {
			return true;
		} else {

			// using min distance
			double dist = sqrt(
					exp(fabs(pos.x - aim.x)) + exp(fabs(pos.y - aim.y)));
			printf("dist2aim: %.0f\n", dist);
			if (dist > dist2aim) {
				return true;
			}
			dist2aim = dist;
		}
	}
	return false;
}

