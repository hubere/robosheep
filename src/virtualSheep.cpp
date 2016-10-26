/*
 * virtualSheep.cpp
 *
 *  Created on: Sep 19, 2012
 *      Author: ubuntu
 */

#include "virtualSheep.h"

#include <stdio.h>


#define pi 3.14159265

virtualSheep::virtualSheep() {
	pos = Point2f(250,300);
	lastpos = Point2f(0, 0);
	dir =45;
	velocity = 1;
	size = Size(10,15);
	color = Scalar(0, 255, 255); // yellow
}

virtualSheep::~virtualSheep() {
	// TODO Auto-generated destructor stub
}

Size virtualSheep::getSize()
{
	return size;
}

void virtualSheep::locatedAt(Point_<int> location)
{

}



void virtualSheep::update() {

	if (pos == Point2f(0, 0))
			return;

	//	radian = degree * (pi/180);
	double radian = dir * (pi / 180);

	double incX = cos(radian);
	double incY = -sin(radian);

	if (dir > 90) {
		incX = cos(pi / 180 - radian);
		incY = sin(pi / 180 - radian);
	}
	if (dir > 180) {
		incX = cos(radian);
		incY = -sin(radian);
	}
	if (dir > 270) {
		incX = cos(radian);
		incY = -sin(radian);
	}

	pos.x += incX * velocity;
	pos.y += incY * velocity;

	if (pos.x < 0) pos.x = 0;
	if (pos.y < 0) pos.y = 0;
	if (pos.x > 500) pos.x = 500;
	if (pos.y > 500) pos.y = 500;


	// print();
}

void virtualSheep::draw(cv::Mat &frame) {
	if (pos == Point2f(0, 0)) return;
	rectangle(frame, cvPoint(pos.x - size.width, pos.y - size.height), cvPoint(pos.x + size.width, pos.y + size.height), color, -1, 8, 0);
	if (lastpos == Point2f(0, 0)) return;
	line(frame, lastpos, pos, cvScalar(255, 0, 0), 2);
	circle(frame, pos, 4, cvScalar(255, 0, 0), 2);
}

Point2f virtualSheep::getPosition() {
	return pos;
}

void virtualSheep::setPosition(Point2f newPos) {
	pos = newPos;
}

void virtualSheep::rotate(double dt) {
	dir += dt;
	if (dir > 360) dir = dir-360;
	if (dir < 0) dir = dir+360;
}

void virtualSheep::speedUp() {
	velocity++;
}

void virtualSheep::slowDown() {
	velocity--;
}

void virtualSheep::print() {
	printf("sheep (%.0f,%.0f): v=%d, d=%d\n", pos.x, pos.y, velocity, dir);
}

