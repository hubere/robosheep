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

#endif /* OPENCVUTILS_H_ */
