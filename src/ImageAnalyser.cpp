/*
 * ImageAnalyser.cpp
 *
 *  Created on: Sep 19, 2012
 *      Author: ubuntu
 */

#include "ImageAnalyser.h"

#include <stdio.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;

#define pi 3.14159265

const char* window_name = "ImageAnalyser";
const char* trackbar_type =
		"Type: \n 0: Binary \n 1: Binary Inverted \n 2: Truncate \n 3: To Zero \n 4: To Zero Inverted";
const char* trackbar_value = "Value";


Mat src, src_gray, dst;

int threshold_value = 0;
int threshold_type = 3;
int const max_value = 255;
int const max_type = 4;
int const max_BINARY_value = 255;

/// Function headers
void Threshold_Demo( int, void* );

ImageAnalyser::ImageAnalyser() {
	pos = Point2f(250, 300);
	lastpos = Point2f(0, 0);
	dir = 45;
	velocity = 1;
	size = Size(10, 15);
	color = Scalar(0, 255, 255); // yellow
}

ImageAnalyser::~ImageAnalyser() {
	// TODO Auto-generated destructor stub
}

void ImageAnalyser::analyse(std::string imageName) {

	/// Load an image
	src = imread(imageName, 1);

	  /// Convert the image to Gray
	  cvtColor( src, src_gray, COLOR_RGB2GRAY );


	// Create a window to display results
	namedWindow(window_name, WINDOW_AUTOSIZE);

	/// Create Trackbar to choose type of Threshold
	createTrackbar(trackbar_type, window_name, &threshold_type, max_type,
			Threshold_Demo);

	createTrackbar(trackbar_value, window_name, &threshold_value, max_value,
			Threshold_Demo);

	/// Call the function to initialize
	  Threshold_Demo( 0, 0 );

	waitKey(0);
}

/**
 * @function Threshold_Demo
 */
void Threshold_Demo( int, void* )
{
  /* 0: Binary
     1: Binary Inverted
     2: Threshold Truncated
     3: Threshold to Zero
     4: Threshold to Zero Inverted
   */

  threshold( src_gray, dst, threshold_value, max_BINARY_value,threshold_type );

  imshow( window_name, dst );
}

Size ImageAnalyser::getSize() {
	return size;
}

void ImageAnalyser::update() {

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

	if (pos.x < 0)
		pos.x = 0;
	if (pos.y < 0)
		pos.y = 0;
	if (pos.x > 500)
		pos.x = 500;
	if (pos.y > 500)
		pos.y = 500;

	// print();
}

void ImageAnalyser::draw(cv::Mat &frame) {
	if (pos == Point2f(0, 0))
		return;
	rectangle(frame, cvPoint(pos.x - size.width, pos.y - size.height),
			cvPoint(pos.x + size.width, pos.y + size.height), color, -1, 8, 0);
	if (lastpos == Point2f(0, 0))
		return;
	line(frame, lastpos, pos, cvScalar(255, 0, 0), 2);
	circle(frame, pos, 4, cvScalar(255, 0, 0), 2);
}

Point2f ImageAnalyser::getPosition() {
	return pos;
}

void ImageAnalyser::setPosition(Point2f newPos) {
	pos = newPos;
}

void ImageAnalyser::rotate(double dt) {
	dir += dt;
	if (dir > 360)
		dir = dir - 360;
	if (dir < 0)
		dir = dir + 360;
}

void ImageAnalyser::speedUp() {
	velocity++;
}

void ImageAnalyser::slowDown() {
	velocity--;
}

void ImageAnalyser::print() {
	printf("sheep (%.0f,%.0f): v=%d, d=%d\n", pos.x, pos.y, velocity, dir);
}

