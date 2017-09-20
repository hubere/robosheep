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
	vector<string> infos;

public:
	GUI();
	virtual ~GUI();
	void addWindow(const string& winname);
	void showImage(const String& winname, InputArray mat);
	void showWindow(int id);
	void printInfo(int line, const std::string& info);
	void deleteInfo(int line);
	void refreshDisplay();
};

#endif /* SRC_GUI_H_ */
