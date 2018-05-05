/*
 * TrackedObject.h
 *
 *  Created on: Oct 26, 2016
 *      Author: edi
 */

#ifndef SRC_TRACKEDCOLORBLOB_H_
#define SRC_TRACKEDCOLORBLOB_H_

#include <opencv2/highgui/highgui.hpp>

#include "GUI.h"

using namespace cv;

namespace robosheep {

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
	CELLAR_RED_CIRCLE =3,
	GARDEN_BLUE_SQUARE,
	GARDEN_ORANGE_SQUARE,
	GARDEN_YELLOW_CIRLCE,
	GARDEN_RED_CIRLCE,
	GARDEN_LED_RED,
	GARDEN_LED_YELLOW,
	GARDEN_BIKE_WHITE,
	GARDEN_BIKE_RED,
	GARDEN_BIKE_RED_AND_WHITE

};




class TrackedColorBlob {
	int high_H;
	int high_S;
	int high_V;
	int low_H;
	int low_S;
	int low_V;
	int color_range;
	Size maxSize;
	Size minSize;
	string name;

public:
	TrackedColorBlob(int datasetidx);
	TrackedColorBlob(string name, int H, int S, int V, int range, Size minSize, Size maxSize);
	TrackedColorBlob(string name, int highH, int highS, int highV, int lowH, int lowS, int lowV, int range, Size minSize, Size maxSize);
	void init(string pName, int highH, int highS, int highV, int range, Size minSize, Size maxSize);
	void init(string pName, int highH, int highS, int highV, int lowH, int lowS, int lowV, int range, Size minSize, Size maxSize);
	virtual ~TrackedColorBlob();
	void show(GUI& gui);
	String getName();
	Scalar getGimpColor();
	Scalar getGimpColorHigh();
	Scalar getGimpColorLow();
	Scalar getGimpColorHigh(int range);
	Scalar getGimpColorLow(int range);
	Size getMinSize();
	Size getMaxSize();
	int getColorRange();
	String toString();

	static void onChange(int v, void *ptr) {
		TrackedColorBlob *that = (TrackedColorBlob*) ptr;
		that->refresh();
	}
	void refresh();

};

}  // namespace robosheep

#endif /* SRC_TRACKEDCOLORBLOB_H_ */
