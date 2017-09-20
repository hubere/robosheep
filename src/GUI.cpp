/*
 * GUI.cpp
 *
 *  Created on: Nov 2, 2016
 *      Author: edi
 */

#include "GUI.h"

#include <opencv2/imgproc/imgproc.hpp>
#include <algorithm>

static const string WINDOW_INFO = "INFO";

GUI::GUI() : nextWindowX(0), nextWindowY(0)
{
	addWindow(WINDOW_INFO);
}

GUI::~GUI() {
	// TODO Auto-generated destructor stub
}

void GUI::addWindow(const string& winname){

	windowNames.push_back(winname);
	namedWindow(winname, WINDOW_AUTOSIZE);
	moveWindow(winname, nextWindowX,nextWindowY);
	nextWindowX += 640;
	if (nextWindowX > 1800)
	{
		nextWindowX = 0;
		nextWindowY += 480;
	}
}

void GUI::showImage(const String& winname, InputArray mat)
{
	if(windowShown == 0 || windowNames[windowShown] == winname)
		imshow(winname,  mat);
}

void GUI::showWindow(int id)
{
	if (id >= windowNames.size()) return;

	destroyAllWindows();
	
	namedWindow(WINDOW_INFO, WINDOW_AUTOSIZE); // info windows is always displayed
	refreshDisplay();

	if (id == 0)
	{
		for (auto &windowName : windowNames) // access by reference to avoid copying
		{
			namedWindow(windowNames[id], WINDOW_AUTOSIZE);
		}
	}
	else
	{
		namedWindow(windowNames[id], WINDOW_AUTOSIZE);
	}
	windowShown = id;
}

void GUI::printInfo(int line, const std::string& info)
{
	infos.resize(std::max((int)infos.size(), line+1));
	infos.at(line) = info;
	refreshDisplay();
}

void GUI::deleteInfo(int line)
{

}

void GUI::refreshDisplay()
{
	// Create black empty images
	Mat image = Mat::zeros(400, 400, CV_8UC3);

	for (std::vector<String>::size_type i = 0; i != infos.size(); i++) {
		putText(image, infos.at(i), Point(10, 20 + i * 12), CV_FONT_HERSHEY_PLAIN, 1.0, Scalar::all(255), 1, CV_AA);
	}

	imshow(WINDOW_INFO, image);
}


