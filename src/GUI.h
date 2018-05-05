/*
 * The GUI provides functionality for displaying images and information. Its 
 * does some layout.
 *
 *  Created on: Nov 2, 2016
 *      Author: Edwin Huber
 */

#ifndef SRC_GUI_H_
#define SRC_GUI_H_

#include <map>
#include <opencv2/highgui.hpp>


using namespace std;
using namespace cv;

namespace robosheep {

class GUI {
	int nextWindowX, nextWindowY;
	vector<string> windowNames;
	int windowShown = 0; // 0 is all windows
	vector<string> infos;
	Mat infoImage;

public:
	GUI();
	virtual ~GUI();
	void addWindow(const string& winname);
	void showImage(const String& winname, Mat& mat);
	void showWindow(int id);
	void printInfo(int line, const std::string& info);
	void printInfo(int line, const std::string& info, int value);
	void deleteInfo(int line);
	void refreshDisplay();
	Mat& getInfoImage();
};

}  // namespace robosheep

#endif /* SRC_GUI_H_ */
