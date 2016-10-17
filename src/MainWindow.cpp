/*
 * MainWindow.cpp
 *
 *  Created on: Oct 17, 2016
 *      Author: edi
 */

#include "MainWindow.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>


using namespace cv;


MainWindow::MainWindow() {
	// TODO Auto-generated constructor stub
}

MainWindow::~MainWindow() {
	// TODO Auto-generated destructor stub
}

void MainWindow::show(){

	namedWindow("MainWindow");
}

