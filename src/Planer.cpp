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
#include <iostream>

using namespace std;
using namespace cv;



static const string WINDOW_PLANER = "Planer";
static const bool useDistance = false;
static double dist2aim = std::numeric_limits<double>::max();

void mouseCallBackPlaner(int event, int x, int y, int flags, void* userdata);


int numberPositions = 10;

Planer::Planer(TrackedObject &pTrackedObject) :
		aim(0, 0),
		proximity(5),
		trackedObject(pTrackedObject),
		velocity(0),
		rotate(),
		motorSpeed1(0),
		motorSpeed2(0) {
}

Planer::~Planer() {
}

void Planer::show(GUI& gui) {
	gui.addWindow(WINDOW_PLANER);
	createTrackbar(" trajectory length:", WINDOW_PLANER, &numberPositions, 100);
	setMouseCallback(WINDOW_PLANER, mouseCallBackPlaner, this);
}


void Planer::show(Mat& frame) {
	Mat planned;
	frame.copyTo(planned);

	vector<Point2i> lastPositions = trackedObject.getPositionHistory();
	if (lastPositions.size() < 1) return;

	//
	// draw aim
	//
	circle(planned, aim, 5, BGR_YELLOW, 3);

	//
	// draw last detected position
	//
	Point roboPos = lastPositions.back();
	circle(planned, roboPos, 5, BGR_GREEN, 3);

	//
	// draw last positions
	//
	int myradius=2;
	for (int i=0;i<(int)lastPositions.size();i++)
	    circle(planned,Point(lastPositions[i].x,lastPositions[i].y),myradius,BGR_RED,1);

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
	line(planned, startPoint, endPoint, BGR_PINK, 1);

	//
	// print data
	//
	int line = 0;
	ostringstream text;
	text << "aim:       " << aim;
	putText(planned, line++, text.str(), BGR_YELLOW);

	text.str("");
	text << "roboPos:   " << roboPos;
	putText(planned, line++, text.str(), BGR_GREEN);

	text.str("");
	text << "lastPositions: ";
	for (std::vector<Point2i>::const_iterator i = lastPositions.begin(); i != lastPositions.end(); ++i)
		text << *i << ", ";
	text << std::endl;
	putText(planned, line++, text.str(), BGR_RED);

	text.str("");
	text << "boundingRect: " << r;
	putText(planned, line++, text.str());

	text.str("");
	text << "fittedline: " << fittedline;
	putText(planned, line++, text.str(), BGR_PINK);

	imshow(WINDOW_PLANER, planned);
}

void Planer::putText(Mat& frame, int line, const string& text) {
	putText(frame, line, text, Scalar::all(255));
}

void Planer::putText(Mat& frame, int line, const string& text, Scalar color) {
	cv::putText(frame, text, Point(10,20+line*12), CV_FONT_HERSHEY_PLAIN, 1.0, color, 1, CV_AA);
}

void Planer::setAim(Point2f newAim) {
	aim = newAim;
}

Point2f Planer::getAim() {
	return aim;
}

int Planer::getMotorSpeed1() {
	return motorSpeed1;
}

int Planer::getMotorSpeed2() {
	return motorSpeed2;
}

void Planer::speedUp() {
	velocity += 10;
	cout << "Planer::speedUp: velocity=" << velocity;
}

void Planer::slowDown() {
	velocity -=10;
	cout << "Planer::slowDown: velocity=" << velocity;
}

int Planer::plan() {
	OpenCVUtils util;
	Point2i aktPos = trackedObject.getAktualPos();

	//
	// 1. calc direction to head
	//
	// int tiDegree2 = util.getKurswinkelDegree(lastPos, aktPos); // Kurswinkel ist aus letzter Position
	int tiDegree = trackedObject.getKurswinkelDegree(); // Kurswinkel ist aus ImageAnalyser
	int tsDegree = util.getKurswinkelDegree(aktPos, aim); // Kurswinkel soll

	//
	// 2. calc movement commands
	//
	rotate = (tsDegree - tiDegree);
	if (rotate > 180)
		rotate = rotate - 360;
	if (rotate < -180)
		rotate = 360 + rotate;

	//
	// 3. calc motor speeds
	//
	// the following lines calculate the actual motor speed by measuring the distance between lastPos and aktPos!
	//	int speed = (int) sqrt(
	//			(lastPos.x - aktPos.x) * (lastPos.x - aktPos.x)
	//					+ (lastPos.y - aktPos.y) * (lastPos.y - aktPos.y));
	motorSpeed1 = velocity + rotate;
	motorSpeed2 = velocity - rotate;

//	if (dist < 10) {
//		// sheep is not moving! => do not rotate
//		rotate = 0;
//	}

	cout << "Planer::plan:	aim=("<<aim<<")" << endl;
	cout << "Planer::plan:	ti="<<tiDegree<<" ts="<<tsDegree<<" => rotate:"<< rotate << endl;
	cout << "Planer::plan:	speed="<<velocity<<" m1="<<motorSpeed1<<" m2="<<motorSpeed2<< endl;
	if (velocity == 0)
		cout << "Planer::plan:	hit '+' and '-' to increase and decrease speed." << endl;

	return rotate;
}

bool Planer::isRoutePointReached() {
	Point2f pos = trackedObject.getAktualPos();
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

void mouseCallBackPlaner(int event, int x, int y, int flags, void* userdata) {
	Planer* pPlaner = (Planer*) userdata;
	if (event == EVENT_LBUTTONDOWN) {


	} else if (event == EVENT_RBUTTONDOWN) {
		pPlaner->setAim(Point(x,y));
		cout << "Planer: setting new aim to (" << x << ", " << y << ")" << endl;
	} else if (event == EVENT_MBUTTONDOWN) {
		cout << "Planer::help	right mouse button sets new aim." << endl;
	} else if (event == EVENT_MOUSEMOVE) {
		// cout << "Mouse move over the window - position (" << x << ", " << y << ")" << endl;
	}

}

