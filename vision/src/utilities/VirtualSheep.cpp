/*
 * VirtualSheep.cpp
 *
 *  Created on: Sep 19, 2012
 *      Author: ubuntu
 */

#include "VirtualSheep.h"
#include "../presentation/GUI.h"

#include <stdio.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace cv;
using namespace std;

namespace robosheep {

//-----------------------------------------------
// Constants
//-----------------------------------------------

#define pi float(3.14159265)
static const string WINDOW_VIRTUAL_SHEEP = "Virtual Sheep";


VirtualSheep::VirtualSheep() {
	pos = Point2f(250, 300);
	lastpos = Point2f(0, 0);
	speedM1 = 0;
	speedM2 = 0;
	dir = 45;
	velocity = 1;
	size = Size(10, 15);
	color = Scalar(0, 255, 255); // yellow
}

VirtualSheep::~VirtualSheep() {
	// TODO Auto-generated destructor stub
}

void VirtualSheep::show(GUI& gui) {
	gui.addWindow(WINDOW_VIRTUAL_SHEEP);
}

Size VirtualSheep::getSize() {
	return size;
}

Scalar VirtualSheep::getColor(){
	return color;
}

void VirtualSheep::locatedAt(Point_<int> location) {

}

void VirtualSheep::update() {

	if (pos == Point2f(0, 0))
		return;

	float radian = dir * (pi / 180);
	float incX = cos(radian);
	float incY = -sin(radian);

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

	if (pos.x < 0)
		pos.x = 0;
	if (pos.y < 0)
		pos.y = 0;
	if (pos.x > 500)
		pos.x = 500;
	if (pos.y > 500)
		pos.y = 500;

}

void VirtualSheep::draw(Mat &frame) {

	//
	// draw sheep in original frame
	//
	if (pos != Point2f(0, 0)) {
		rectangle(frame, cv::Point((int)pos.x - size.width, (int)pos.y - size.height),
				cv::Point((int)pos.x + size.width, (int)pos.y + size.height), color, -1, 8,
				0);
		if (lastpos != Point2f(0, 0)) {
			line(frame, lastpos, pos, cv::Scalar(255, 0, 0), 2);
			circle(frame, pos, 4, cv::Scalar(255, 0, 0), 2);
		}
	}

	//
	// ouput text in copy of frame
	//
	Mat newFame;
	frame.copyTo(newFame);
	char buffer[100];
	sprintf(buffer, "Use 'w','y','a','s','+','-' to control sheep");
	putText(newFame, buffer, Point(40,50), FONT_HERSHEY_COMPLEX_SMALL, 1, Scalar::all(255), 1, 8);
	sprintf(buffer, "(%.0f,%.0f): v=%d, d=%d\n", pos.x, pos.y, velocity, dir);
	putText(newFame, buffer, Point(40,100), FONT_HERSHEY_COMPLEX_SMALL, 1, Scalar::all(255), 1, 8);

	imshow(WINDOW_VIRTUAL_SHEEP, newFame);
}

Point2f VirtualSheep::getPosition() {
	return pos;
}

void VirtualSheep::setPosition(Point2f newPos) {
	pos = newPos;
}

/**
* Rotate sheep by dt degrees.
*/
void VirtualSheep::rotate(int dt) {
	dir += dt;
	if (dir > 360)
		dir = dir - 360;
	if (dir < 0)
		dir = dir + 360;
}

void VirtualSheep::setSpeed(int m1, int m2)
{
	speedM1 = m1;
	speedM2 = m2;
}

void VirtualSheep::speedUp() {
	velocity++;
}

void VirtualSheep::slowDown() {
	velocity--;
}

void VirtualSheep::print() {
	printf("sheep (%.0f,%.0f): v=%d, d=%d\n", pos.x, pos.y, velocity, dir);
}

void VirtualSheep::keyPressed(char key) {

	switch (key) {
	case 'w':
		dir = 90;
		break;
	case 'y':
		dir = 270;
		break;
	case 'a':
		dir = 180;
		break;
	case 's':
		dir = 0;
		break;
	case 'q': // set sheep to start position
		setPosition(Point2f(250, 300));
		break;
	case '+': // speed up sheep
		speedUp();
		break;
	case '-': // slow down sheep
		slowDown();
		break;
	}

}

}  // namespace robosheep
