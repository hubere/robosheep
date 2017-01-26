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


class GUI {
	int nextWindowX, nextWindowY;
public:
	GUI();
	virtual ~GUI();
	void addWindow(const string& winname);
};

#endif /* SRC_GUI_H_ */
