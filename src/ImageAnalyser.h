/*
 * ImageAnalyser.h
 *
 *  Created on: Sep 19, 2012
 *      Author: ubuntu
 */

#ifndef ImageAnalyser_H_
#define ImageAnalyser_H_

#include <opencv2/core/core.hpp>

using namespace std;
using namespace cv;

class ImageAnalyser {
	// position and direction of sheep
	Point2f pos;
	Point2f lastpos;
	int dir;
	int velocity;
	Size size;
	Scalar color;

public:
	ImageAnalyser();
	virtual ~ImageAnalyser();
	void analyse(std::string imageName);
	void draw(cv::Mat &frame);
	Size getSize();
	Point2f getPosition();
	void setPosition(Point2f newPos);
	void rotate(double dt);
	void speedUp();
	void slowDown();
	void update();
	void print();
};

#endif /* ImageAnalyser_H_ */
