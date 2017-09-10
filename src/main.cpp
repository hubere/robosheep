#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/video/tracking.hpp>
#include <stdio.h>
#include <sstream>
#include <math.h>
#include <unistd.h>
#include <limits>
#include <iostream>
#include <string>
#include <vector>
#include <time.h>

// Network related include
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>


#include "MainWindow.h"
#include "OpenCVUtils.h"
#include "ImageAnalyser.h"
#include "VideoCamera.h"
#include "Garden.h"
#include "GUI.h"
#include "VirtualSheep.h"
#include "Planer.h"
#include "HTTPClient.h"

using namespace std;
using namespace cv;

// our virtual sheep
VirtualSheep sheep;
OpenCVUtils util;

static int idx = 0;

void mouseHandler(int event, int x, int y, int flags, void *param) {
	switch (event) {
	/* left button down */
	case CV_EVENT_LBUTTONDOWN:
//		fprintf(stdout, " Left button down (%d, %d).\n", x, y);
		fprintf(stdout, "route[%i] = Point(%d, %d);\n", idx++, x, y);
		break;
	}
}


string evaluateArgs(int argc, char** argv, string tag, string defaultValue) {
	for (int i=0; i< argc; i++)
	{
		if (argv[i] == tag)
			return argv[i+1];
	}
	return defaultValue;
}

void printCallingParameter(int argc, char** argv) {
	//
	// print calling arguments
	//
	cout << "main: === started robosheep ===" << endl;
	for (int i = 0; i < argc; i++)
		cout << "main: arg[" << i << "] " << argv[i] << endl;
	cout << endl;
}

int main(int argc, char** argv) {

	//
	// print calling arguments
	//
	printCallingParameter(argc, argv);

	//
	// evaluate arguments
	//
	string argCameraURL = evaluateArgs(argc, argv, "--cameraURL", "noCameraURL");
//	string argCameraURL = evaluateArgs(argc, argv, "--cameraURL", "rtsp://192.168.1.111:554/onvif1");

	string argImageName = evaluateArgs(argc, argv, "--image", "snapshot");
	string argSimulateSheep = evaluateArgs(argc, argv, "--simulateSheep", "false");
	string argMowerURL = evaluateArgs(argc, argv, "--mowerURL", "http://localhost/");



	if (argc > 1) {
		std::string arg1 = argv[1];

		if (arg1 == "--testHTTP") {

			HTTPClient client(argMowerURL);
			client.sendMessage("motor?m1=10&m2=50");
			exit(0);
		}

		if ((arg1 == "-t") || (arg1 == "--track")) {

			//
			// we want to track an object
			//

			VideoCamera videoCamera;
			TrackedObject trackedObject;
			ImageAnalyser imageAnalyser;

			Mat frame;
			int framedelay = 1000;

			while (videoCamera.read(frame, framedelay)) {
				imageAnalyser.detectObjectPosition(frame, trackedObject);
			}

			exit(0);
		}

		if (arg1 == "--analyse") {

			MainWindow mainWindow;
			mainWindow.start();
		}


	}

	if (argc > 2) {
		std::string arg1 = argv[1];
		std::string arg2 = argv[2];

		if ((arg1 == "-i") || (arg1 == "--analyseImage")) {

			//
			// we want to analyse an image
			//

			GUI gui;
			TrackedObject trackedObject;
			ImageAnalyser imageAnalyser;

			trackedObject.show(gui);
			imageAnalyser.show(gui);
			imageAnalyser.analyse(argImageName, trackedObject);

			exit(0);
		}

		if (arg1 == "--virtualSheep") {

			//
			// we want to track a virtualSheep
			//

			GUI gui;
			VideoCamera videoCamera;
			TrackedObject trackedObject;
			ImageAnalyser imageAnalyser;
			VirtualSheep virtualSheep;

//			trackedObject.setGimpColor(
//					util.openCV2gimpValue(virtualSheep.getColor()));

			virtualSheep.show(gui);
			imageAnalyser.show(gui);

			bool stop(false);
			Mat origFrame, frame;
			int framedelay = 1000;

			// read next frame if any
			if (!videoCamera.read(origFrame)) {
				exit(-1);
			}

			while (!stop) {

				origFrame.copyTo(frame);
				virtualSheep.update();
				virtualSheep.print();
				virtualSheep.draw(frame);

 				imageAnalyser.detectObjectPosition(frame, trackedObject);

				char key = cv::waitKey(framedelay);
				virtualSheep.keyPressed(key);

			}

			exit(0);
		}

		else
		{

			//
			// control the mower
			//

			//
			// initialize variables
			//
			Mat frame;	// actual image of camera

			// Each frame should be processes in same amount of time, the 'frametime'. The time
			// consumed by a frame processing round trip is substracted from 'frametime' and stored
			// in 'framedelay'. The next round trip will start by waiting 'framedelay' ms. Hence
			// a frame will be processed each 'frametime' ms.
			//int frametime = 5000; 			 	// take one frame each 1000ms <= too big for a stream ?!!?
			int frametime = 200; 			 	// take one frame each 1000ms
			double frameProcessingStart = (double)getTickCount();; 	// for measuring duration of frame processing loop
			int framedelay = 10; 				// the delay for next round trip in oder to reach a 'frametime'
												// (must be more than 0 in order to not stop program)

			bool stop(false);		// flag to stop the endless loop
			int routeIdx = 0;		// index into sequence of rout points, starting with first rout point
			//Point_<int> roboPos;	// actual position of sheep

			// These are not used?!
			// RNG rng(12345);
			// int thresh = 100;


			//
			// instantiate classes
			//
			GUI gui;
			Garden garden(GARDEN);
			VideoCamera videoCamera;
			TrackedObject trackedObject;
			ImageAnalyser imageAnalyser;
			Planer planer(trackedObject);
			HTTPClient client(argMowerURL);

			//
			// initialize camera
			// open camera stream or load image
			//
			if (argCameraURL != "noCameraURL"){
				if (!videoCamera.open(argCameraURL)) exit(-1);
			}
			else{
				videoCamera.loadImage(argImageName);
			}
			if (!videoCamera.read(frame))  return 0;
			videoCamera.show(gui);


			//
			// initialize garden
			//
			garden.show(gui);


//			trackedObject.show(gui);
			imageAnalyser.show(gui);

			// tell planer to where sheep should move
			planer.show(gui);
			planer.setAim(garden.getRoutePoint(routeIdx));

			//
			// for ever....
			//
			while (!stop) {

				//
				// calc new framedelay
				//
				int algtime = ((double)getTickCount() - frameProcessingStart)*1000./getTickFrequency();
				if (frametime > algtime)
					framedelay = frametime - algtime;
				else
					framedelay = 10;	// minimal framedelay

				//
				// wait and get user keyboard input
				//
				char key = cv::waitKey(framedelay);
				cout << endl << "-----------------------------------------------------" << endl;
				cout << "main:		algtime="<<algtime<<" framedelay=" << framedelay << endl;
				frameProcessingStart = (double)getTickCount();
				switch (key) {
				case 27:
					stop = true;
					break;
				case 'n':
					// unmow
//					frame.copyTo(mowed);
//					garden.showGreen(mowed);
//					garden.showRoute(mowed);
					break;
				case 's': // set sheep to start position
					sheep.setPosition(Point2f(250, 300));
					break;
				case '+': // speed up sheep
					sheep.speedUp();
					planer.speedUp();
					break;
				case '-': // slow down sheep
					sheep.slowDown();
					planer.slowDown();
					break;
				}

				//
				// read next frame if any
				//
				if (!videoCamera.read(frame)) {
					printf("Could not read frame from camera.\n");
					break;
				}
				planer.show(frame);
				garden.setImage(frame);


				//
				// just for simulation: draw sheep in camera frame
				//
				if (argSimulateSheep == "true")
				{
					sheep.update();
					sheep.draw(frame);
				}

				//
				// get position of tracked object (sheep)
				//
				Mat greenImage = garden.maskOutGreen(frame);
				bool objectDetected = imageAnalyser.detectObjectPosition(greenImage, trackedObject);
				if (!objectDetected) continue;

				//
				// update mowed image
				//
//				if (trackedObject.getAktualPos())
//				garden.setMowed(lastPos);
//				if (lastPos.x > 0 && lastPos.y > 0 && roboPos.x > 0
//						&& roboPos.y > 0) {
//					line(mowed, roboPos, lastPos, cvScalar(255, 255, 255), 10);
//				}

				//
				// when aim is reached, get next aim
				//
				if (planer.isRoutePointReached()) {
					routeIdx++;
					if (routeIdx >= (int) garden.getRouteSize())
						stop = true;
					Point2f aim = garden.getRoutePoint(routeIdx);
					planer.setAim(aim);
				}

				//
				// calc and issue steering command
				//
				int rotate = planer.plan();

				//
				// just for simulation: draw sheep in camera frame
				//
				if (argSimulateSheep == "true")
				{
					sheep.rotate(rotate);
					sheep.setSpeed(planer.getMotorSpeed1(), planer.getMotorSpeed2());
					sheep.print();
				}

				// draw direction indicator
				//  TODO FIXME merge:
	//			int length = 50;
	//			Point P2;
	//			P2.x =  (int)round(roboPos.x + length * cos(rotate * CV_PI / 180.0));
	//			P2.y =  (int)round(roboPos.y + length * sin(rotate * CV_PI / 180.0));
	//			Point2f aim = garden.getRoutePoint(routeIdx);
	//			circle(frame, aim, 5, cvScalar(0, 255, 255), 1);
	//			circle(frame, roboPos, 5, cvScalar(0, 255, 0), 1);
	//			line(frame, roboPos, P2, cvScalar(255, 0, 255), 5);
	//			planer.show(frame);
	//

				//
				// send command to roboSheep via http
				// but only if we realy have an image from camera and not a static one
				//
				char command[255];
				sprintf(command, "motor?m1=%d&m2=%d", planer.getMotorSpeed1(), planer.getMotorSpeed2());
				if (argCameraURL != "noCameraURL"){
					client.sendMessage(command);
					cout << "main:		sending client.sendMessage("<< command << ")" << endl;
				}else{
					cout << "main:		would send client.sendMessage("<< command << ")" << endl;
				}


				//-------------------------------------------------------------------------
				// Algorithm Done
				//-------------------------------------------------------------------------

				// show foreground
//				imshow("mowed", mowed);
			}

		}
	};

	return (0);
}

//static void show_usage(std::string name) {
//	std::cerr << "Usage: " << name << " <option(s)> SOURCES" << "Options:\n"
//			<< "\t-h,--help\t\tShow this help message\n"
//			<< "\t-d,--destination DESTINATION\tSpecify the destination path"
//			<< std::endl;
//}

