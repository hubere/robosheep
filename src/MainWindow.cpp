/*
 * MainWindow.cpp
 *
 *  Created on: Oct 17, 2016
 *      Author: edi
 */

#include "MainWindow.h"
#include "GUI.h"
#include "ImageAnalyser.h"
#include "VideoCamera.h"
#include "Garden.h"
#include "VirtualSheep.h"
#include "Planer.h"
#include "HTTPClient.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>


using namespace cv;
using namespace std;


MainWindow::MainWindow() {
	// TODO Auto-generated constructor stub
}

MainWindow::~MainWindow() {
	// TODO Auto-generated destructor stub
}



void MainWindow::start(){


	GUI gui;
	VideoCamera videoCamera;
	ImageAnalyser imageAnalyser;

	Garden garden(GARDEN);
	TrackedObject trackedObject;
//	Planer planer;

	Mat frame;

	//
	// open camera stream
	//
// TODO FIXME 	string cameraURL = evaluateArgs(argc, argv, "--cameraURL", "http://192.168.1.105/video.cgi?x.mjpg");
	String cameraURL = "http://192.168.1.101/video.cgi?x.mjpg";
	if (!videoCamera.open(cameraURL)) exit(-1);
	videoCamera.show(gui);


	std::cout << "\n\n\nUsage:" << std::endl
			<< "	't'		take snapshot" << std::endl
			<< "	'a'		analyse snapshot" << std::endl
			<< "	'ESC'	exit" << std::endl
			<< std::endl;


	bool stop(false);
	int framedelay = 1000;

	while (!stop) {

		char key = waitKey(framedelay);
		switch (key) {
		case 27:
			stop = true;
			break;

		case 't':
			videoCamera.saveFrame();
			break;

		case 'a':
			Mat snapshot;
			videoCamera.takeSnapshot(snapshot);
			imageAnalyser.show(gui);
			imageAnalyser.analyse(snapshot, trackedObject);
			break;
		}

		if (!videoCamera.read(frame)) {
			std::cerr << "Could not read frame\n" << std::endl;
			continue;
		}


	}
}

void MainWindow::show(){

	namedWindow("MainWindow");
}

