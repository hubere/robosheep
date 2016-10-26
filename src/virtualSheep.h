/*
 * virtualSheep.h
 *
 *  Created on: Sep 19, 2012
 *      Author: ubuntu
 */

#ifndef VIRTUALSHEEP_H_
#define VIRTUALSHEEP_H_

#include <opencv2/core/core.hpp>

using namespace std;
using namespace cv;

class virtualSheep {
	// position and direction of sheep
	Point2f pos;
	Point2f lastpos;
	int dir;
	int velocity;
	Size size;
	Scalar color;

public:
	virtualSheep();
	virtual ~virtualSheep();
	void draw(cv::Mat &frame);
	Size getSize();
	Point2f getPosition();
	void setPosition(Point2f newPos);
	void rotate(double dt);
	void speedUp();
	void slowDown();
	void update();
	void print();
	void locatedAt(Point_<int> location);
};

#endif /* VIRTUALSHEEP_H_ */
