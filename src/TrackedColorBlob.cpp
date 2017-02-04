/*
 * TrackedColorBlob.cpp
 *
 *  Created on: Oct 26, 2016
 *      Author: edi
 */

#include "TrackedColorBlob.h"

#include <stdio.h>
#include <iostream>
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
		break;
	}
	case CELLAR_YELLOW_DINA4: {
		color_H = 35; //30-40
		color_S = 30; // 30-30
		color_V = 90; // 80-100
		color_range = 20;
		size = Size(100, 50);
		break;
	}
	case CELLAR_YELLOW_CIRCLE: {
		color_H = 35; //30-40
		color_S = 30; // 30-30
		color_V = 90; // 80-90
		color_range = 20;
		size = Size(40, 30);
		break;
	}
	case CELLAR_RED_CIRCLE: {
		color_H = 360; //30-40
		color_S = 50; // 30-30
		color_V = 90; // 80-90
		color_range = 20;
		size = Size(40, 30);
		break;
	}
	}
}

TrackedColorBlob::~TrackedColorBlob() {
}


Scalar TrackedColorBlob::getGimpColor() {
	return Scalar(color_H, color_S, color_V);
}


Size TrackedColorBlob::getSize() {
	return size;
}

int TrackedColorBlob::getColorRange() {
	return color_range;
}



