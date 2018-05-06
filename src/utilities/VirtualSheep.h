/*
 * virtualSheep.h
 *
 *  Created on: Sep 19, 2012
 *      Author: ubuntu
 */

#ifndef VIRTUALSHEEP_H_
#define VIRTUALSHEEP_H_

#include "presentation/GUI.h"

#include <opencv2/core/core.hpp>

using namespace std;
using namespace cv;

namespace robosheep {

class VirtualSheep {
	// position and direction of sheep
	Point2f pos;
	Point2f lastpos;
	int dir;
	int velocity;
	Size size;
	Scalar color;
	int speedM1;
	int speedM2;

public:
	VirtualSheep();
	virtual ~VirtualSheep();
	void show(GUI& gui);
	void draw(Mat &frame);
	Size getSize();
	Point2f getPosition();
	Scalar getColor();
	void setPosition(Point2f newPos);
	void rotate(int dt);
	void setSpeed(int m1, int m2);
	void speedUp();
	void slowDown();
	void update();
	void print();
	void locatedAt(Point_<int> location);

	void keyPressed(char key);
};

}  // namespace robosheep

#endif /* VIRTUALSHEEP_H_ */
