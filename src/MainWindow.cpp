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

	Garden garden;
	TrackedObject trackedObject;
	Planer planer;

	Mat frame;

	videoCamera.show(gui);

	// imageAnalyser.detectObjectPosition(frame, trackedObject);




	std::cout << "\n\n\nUsage:" << std::endl
			<< "	't'		take snapshot" << std::endl
			<< "	'a'		analyse snapshot" << std::endl
			<< "	't'		take snapshot" << std::endl
			<< "	't'		take snapshot" << std::endl
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

		case 'a': {
			Mat snapshot;
			videoCamera.takeSnapshot(snapshot);
			imageAnalyser.show(gui);
			imageAnalyser.analyse(snapshot, trackedObject);
			break;
		}

		case 't': {
			Mat save_img;
			videoCamera.takeSnapshot(save_img);
			if (save_img.empty()) {
				cerr
						<< "Something is wrong with the webcam, could not get frame."
						<< std::endl;
			}
			// Save the frame into a file
			imwrite("snapshot.jpg", save_img); // A JPG FILE IS BEING SAVED
			cout << "Snapshot saved to 'snapshot.jpg'" << std::endl;
			break;
		}
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

