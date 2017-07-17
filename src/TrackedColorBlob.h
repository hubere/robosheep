/*
 * TrackedObject.h
 *
 *  Created on: Oct 26, 2016
 *      Author: edi
 */

#ifndef SRC_TRACKEDCOLORBLOB_H_
#define SRC_TRACKEDCOLORBLOB_H_

#include <opencv2/highgui/highgui.hpp>

using namespace cv;

/* Datasets of color blobs.
 *
 * Use robosheep --analyse and type 't' to take a snapshot.
 * Open the snapshot in gimp, choose color picker and doubleclick on selected color.
 * That will open a color wheel where one can read HSV values.
 *  */
enum
{
	GARDEN_YELLOW_DINA4 =0,
	CELLAR_YELLOW_DINA4 =1,
	CELLAR_YELLOW_CIRCLE  =2,
	CELLAR_RED_CIRCLE =3
};


class TrackedColorBlob {
	int color_H;
	int color_S;
	int color_V;
	int color_range;
	Size size;
	string name;

public:
	TrackedColorBlob(int datasetidx);
	virtual ~TrackedColorBlob();
	//String getName();
	Scalar getGimpColor();
	Size getSize();
	int getColorRange();
	String toString();
};

#endif /* SRC_TRACKEDCOLORBLOB_H_ */
