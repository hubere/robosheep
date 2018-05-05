/*
 * GUI.cpp
 *
 *  Created on: Nov 2, 2016
 *      Author: Edwin Huber
 */

#include "GUI.h"

#include <opencv2/imgproc/imgproc.hpp>
#include <algorithm>

static const string WINDOW_INFO = "INFO";

GUI::GUI() : nextWindowX(0), nextWindowY(0)
{
	addWindow(WINDOW_INFO);
	windowShown = 0;
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

void GUI::showImage(const String& winname, Mat& mat)
{
	if(windowShown == 0 || windowNames[windowShown] == winname)
		if (!mat.empty())
			imshow(winname,  mat);
}

void GUI::showWindow(int id)
{
	if (id >= windowNames.size()) return;

//	destroyAllWindows();
	
//	namedWindow(WINDOW_INFO, WINDOW_AUTOSIZE); // info windows is always displayed
	refreshDisplay();

	if (id == 0)
	{
		for (auto &windowName : windowNames) // access by reference to avoid copying
		{
//			namedWindow(windowNames[id], WINDOW_AUTOSIZE);
		}
	}
	else
	{
//		namedWindow(windowNames[id], WINDOW_AUTOSIZE);
		moveWindow(windowNames[id], 400, 0);
		printInfo(5, "Showing " + windowNames[id]);
	}
	windowShown = id;
}

void GUI::printInfo(int line, const std::string& info)
{
	infos.resize(std::max((int)infos.size(), line + 1));
	infos.at(line) = info;
	refreshDisplay();
}

void GUI::printInfo(int line, const std::string& info, int value)
{
	std::ostringstream oss;
	oss << info << value ;
	printInfo(line, oss.str());
}

void GUI::deleteInfo(int line)
{

}

void GUI::refreshDisplay()
{
	// Create black empty images
	infoImage = Mat::zeros(480, 640, CV_8UC3);

	for (std::vector<String>::size_type i = 0; i != infos.size(); i++) {
		putText(infoImage, 
			infos.at(i), 
			Point(10, 20 + int(i) * 12), 
			CV_FONT_HERSHEY_PLAIN, 1.0, 
			Scalar::all(255), 
			1, 
			CV_AA);
	}

	imshow(WINDOW_INFO, infoImage);
}

Mat& GUI::getInfoImage()
{
	return infoImage;
}

