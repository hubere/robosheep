/*
 * TrackedObject.cpp
 *
 *  Created on: Oct 26, 2016
 *      Author: edi
 */

#include <stdio.h>

#include "TrackedObject.h"

using namespace std;

TrackedObject::TrackedObject() {

	size = Size(10,15);
	gimpColorValue = Scalar(70, 30, 97); // yellow
	colorRange = 20;
}

TrackedObject::~TrackedObject() {
	// TODO Auto-generated destructor stub
}

Size TrackedObject::getSize()
{
	return size;
}

int TrackedObject::getColorRange()
{
	return colorRange;
}


Point_<int> TrackedObject::getLastPos() {
	return lastPos;
}

Point_<int> TrackedObject::getAktualPos() {
	return pos;
}

Point_<int> TrackedObject::setAktualPos(Point_<int> newPos) {
	lastPos = pos;
	pos = newPos;

	printf(" TrackedObject : last=(%d,%d) detected=(%d,%d) \n", lastPos.x, lastPos.y, pos.x, pos.y);

	return lastPos;
}

Scalar TrackedObject::getGimpColor() {
	return gimpColorValue;
}

void TrackedObject::print()
{

}


