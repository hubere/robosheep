/*
 * ImageAnalyser.h
 *
 *  Created on: Sep 19, 2012
 *      Author: ubuntu
 */

#ifndef ImageAnalyser_H_
#define ImageAnalyser_H_

#include <opencv2/core/core.hpp>

#include "TrackedObject.h"


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
	TrackedObject* trackedObject;


public:
	static ImageAnalyser& instance()
	{
		static ImageAnalyser INSTANCE;
		return INSTANCE;
	}
	virtual ~ImageAnalyser();
	bool detectObjectPosition(Mat& frame, TrackedObject& trackedObject);
	bool detectObjectPosition(Mat& frame);
	void analyse(std::string imageName, TrackedObject& aTrackedObject);

	void draw(cv::Mat &frame);
	Size getSize();
	Point2f getPosition();
	void setPosition(Point2f newPos);
	void rotate(double dt);
	void speedUp();
	void slowDown();
	void update();
	void print();

private:
	ImageAnalyser();
 	ImageAnalyser(const ImageAnalyser&);
	void operator=(const ImageAnalyser&);

};

#endif /* ImageAnalyser_H_ */
