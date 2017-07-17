/*
 * TrackedColorBlob.cpp
 *
 *  Created on: Oct 26, 2016
 *      Author: edi
 */

#include "TrackedColorBlob.h"

#include <stdio.h>
#include <iostream>
#include <sstream>
#include <string>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;

TrackedColorBlob::TrackedColorBlob(int datasetidx) {
	switch (datasetidx) {

	case GARDEN_YELLOW_DINA4: {
		color_H = 70;
		color_S = 30;
		color_V = 97;
		color_range = 20;
		size = Size(10, 15);
		name = "GARDEN_YELLOW_DINA4";
		break;
	}
	case CELLAR_YELLOW_DINA4: {
		color_H = 35; //30-40
		color_S = 30; // 30-30
		color_V = 90; // 80-100
		color_range = 20;
		size = Size(100, 50);
		name = "CELLAR_YELLOW_DINA4";
		break;
	}
	case CELLAR_YELLOW_CIRCLE: {
		color_H = 35; //30-40
		color_S = 30; // 30-30
		color_V = 90; // 80-90
		color_range = 20;
		size = Size(40, 30);
		name = "CELLAR_YELLOW_CIRCLE";
		break;
	}
	case CELLAR_RED_CIRCLE: {
		color_H = 360; //30-40
		color_S = 50; // 30-30
		color_V = 90; // 80-90
		color_range = 20;
		size = Size(40, 30);
		name = "CELLAR_RED_CIRCLE";
		break;
	}
	}
}

TrackedColorBlob::~TrackedColorBlob() {
}


//String TrackedColorBlob::getName() {
//	return name;
//}

Scalar TrackedColorBlob::getGimpColor() {
	return Scalar(color_H, color_S, color_V);
}


Size TrackedColorBlob::getSize() {
	return size;
}

int TrackedColorBlob::getColorRange() {
	return color_range;
}

String TrackedColorBlob::toString(){

	std::stringstream ss;
//	ss << "TrackedColorBlob: color: " << name <<" ( " << color_H << "," << color_S << "," << color_V;
	ss << "TrackedColorBlob: color: ( " << color_H << "," << color_S << "," << color_V;
	ss << ") color_range: " << color_range << " size: " << size << endl;
	std::string s = ss.str();
	return s;
}


