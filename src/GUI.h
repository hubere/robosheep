/*
 * GUI.h
 *
 *  Created on: Nov 2, 2016
 *      Author: edi
 */

#ifndef SRC_GUI_H_
#define SRC_GUI_H_

#include <opencv2/highgui/highgui.hpp>


using namespace std;
using namespace cv;


namespace robosheep {

// names of windows
static const string WINDOW_SOURCE = "Source";
static const string WINDOW_THRESHED = "Threshed";
static const string WINDOW_CONTOURS = "Contours";


class GUI {
public:
	GUI();
	virtual ~GUI();
};

} /* namespace robosheep */

#endif /* SRC_GUI_H_ */
