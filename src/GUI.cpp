/*
 * GUI.cpp
 *
 *  Created on: Nov 2, 2016
 *      Author: edi
 */

#include "GUI.h"


GUI::GUI() : nextWindowX(0), nextWindowY(0)
{

//	namedWindow(WINDOW_SOURCE, WINDOW_AUTOSIZE);
//	moveWindow(WINDOW_SOURCE, 0,0);
//
//	namedWindow(WINDOW_THRESHED, WINDOW_AUTOSIZE);
//	moveWindow(WINDOW_THRESHED, 600,0);
//
//	namedWindow(WINDOW_CONTOURS, WINDOW_AUTOSIZE);
//	moveWindow(WINDOW_CONTOURS, 1400,0);

}

GUI::~GUI() {
	// TODO Auto-generated destructor stub
}

void GUI::addWindow(const string& winname){
	namedWindow(winname, WINDOW_AUTOSIZE);
	moveWindow(winname, nextWindowX,nextWindowY);
	nextWindowX += 600;
	if (nextWindowX > 1800)
	{
		nextWindowX = 0;
		nextWindowY += 400;
	}
}


