/*
 * OpenCVUtils.h
 *
 *  Created on: Oct 2, 2012
 *      Author: ubuntu
 */

#ifndef OPENCVUTILS_H_
#define OPENCVUTILS_H_

#include <opencv2/highgui/highgui.hpp>

using namespace cv;


class OpenCVUtils {
public:
	OpenCVUtils();
	virtual ~OpenCVUtils();
	VideoCapture openVideoCapture();
	void printWorkingDirectory();
	double getKurswinkel(Point2f Aufpunkt, Point2f Zielpunkt);
	int getKurswinkelDegree(Point2f Aufpunkt, Point2f Zielpunkt);
	void testKurswinkel();
	bool isFileReadable(const char* filename);
	Scalar gimpValue2OpenCV(Scalar gimpValue, int range);
	Scalar openCV2gimpValue(Scalar gimpValue);

};

static const Scalar BGR_BLACK 	= Scalar(  0,  0,  0);
static const Scalar BGR_WHITE 	= Scalar(255,255,255);
static const Scalar BGR_YELLOW 	= Scalar(  0,255,255);
static const Scalar BGR_BLUE 	= Scalar(255,  0,  0);
static const Scalar BGR_GREEN 	= Scalar(  0,255,  0);
static const Scalar BGR_RED	 	= Scalar(  0,  0,255);
static const Scalar BGR_PINK 	= Scalar(255,  0,255);


#endif /* OPENCVUTILS_H_ */
