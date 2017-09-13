/*
 * GUI.h
 *
 *  Created on: Nov 2, 2016
 *      Author: edi
 */

#ifndef SRC_GUI_H_
#define SRC_GUI_H_


#include <opencv2/highgui.hpp>
#include <map>


using namespace std;
using namespace cv;


class GUI {
	int nextWindowX, nextWindowY;
	vector<string> windowNames;
	int windowShown = 0; // 0 is all windows

public:
	GUI();
	virtual ~GUI();
	void addWindow(const string& winname);
	void showImage(const String& winname, InputArray mat);
	void showWindow(int id);
};

#endif /* SRC_GUI_H_ */
