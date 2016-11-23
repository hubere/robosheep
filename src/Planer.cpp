/*
 * Planer.cpp
 *
 *  Created on: Nov 10, 2016
 *      Author: edi
 */

#include "Planer.h"
#include "OpenCVUtils.h"

#include <stdio.h>
#include <math.h>


using namespace std;
using namespace cv;


namespace robosheep {


static const string WINDOW_PLANER = "Planer";
const bool useDistance = false;
static double dist2aim = std::numeric_limits<double>::max();



Planer::Planer() : aim(0,0), proximity(5), speed(0), motorSpeed1(0), motorSpeed2(0) {
}

Planer::~Planer() {
	// TODO Auto-generated destructor stub
}

void Planer::show(GUI& gui) {
	gui.addWindow(WINDOW_PLANER);
}

void Planer::setAim(Point2f newAim)
{
	aim = newAim;
	printf("\nPlaner::setAim(%i,%i)\n", aim.x, aim.y);
}

int  Planer::getMotorSpeed1(){
	return motorSpeed1;
}

int  Planer::getMotorSpeed2(){
	return motorSpeed2;
}

int Planer::plan(Point2f lastPos, Point2f aktPos){
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
	int dist = (int)sqrt((lastPos.x - aktPos.x)*(lastPos.x - aktPos.x) + (lastPos.y-aktPos.y)*(lastPos.y-aktPos.y));
	motorSpeed1 = dist + rotate/10;
	motorSpeed2 = dist - rotate/10;


	printf("\nPlaner::plan aim=(%d,%d)\n", (int)aim.x, (int)aim.y);
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



} /* namespace robosheep */
