/*
 * GUI.cpp
 *
 *  Created on: Nov 2, 2016
 *      Author: edi
 */

#include "GUI.h"

namespace robosheep {

GUI::GUI() {

	namedWindow(WINDOW_SOURCE, WINDOW_AUTOSIZE);
	moveWindow(WINDOW_SOURCE, 0,0);

	namedWindow(WINDOW_THRESHED, WINDOW_AUTOSIZE);
	moveWindow(WINDOW_THRESHED, 600,0);

	namedWindow(WINDOW_CONTOURS, WINDOW_AUTOSIZE);
	moveWindow(WINDOW_CONTOURS, 1400,0);

}

GUI::~GUI() {
	// TODO Auto-generated destructor stub
}

} /* namespace robosheep */
