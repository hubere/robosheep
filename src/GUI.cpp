/*
 * GUI.cpp
 *
 *  Created on: Nov 2, 2016
 *      Author: edi
 */

#include "GUI.h"


GUI::GUI() : nextWindowX(0), nextWindowY(0)
{
}

GUI::~GUI() {
	// TODO Auto-generated destructor stub
}

void GUI::addWindow(const string& winname){
	namedWindow(winname, WINDOW_AUTOSIZE);
	moveWindow(winname, nextWindowX,nextWindowY);
	nextWindowX += 640;
	if (nextWindowX > 1800)
	{
		nextWindowX = 0;
		nextWindowY += 480;
	}
}


