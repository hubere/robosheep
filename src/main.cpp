#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/video/tracking.hpp>
#include <stdio.h>
#include <sstream>
#include <math.h>
#include <limits>
#include <iostream>
#include <string>
#include <vector>
#include <time.h>

// Network related include
//#include <sys/socket.h>
//#include <netdb.h>
//#include <arpa/inet.h>


#include "MainWindow.h"
#include "OpenCVUtils.h"
#include "ImageAnalyser.h"
#include "VideoCamera.h"
#include "Garden.h"
#include "GUI.h"
#include "VirtualSheep.h"
#include "Planer.h"
#include "HTTPClient.h"
#include "Stopwatch.h"
#include "main.h"

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
	String argMode = evaluateArgs(argc, argv, "--mode", "modeControlSheep");

	String argCameraURL = evaluateArgs(argc, argv, "--cameraURL", "noCameraURL");
	//	String argCameraURL = evaluateArgs(argc, argv, "--cameraURL", "rtsp://192.168.1.111:554/onvif1");

	String argImageName = evaluateArgs(argc, argv, "--image", "snapshot");
	String argSimulateSheep = evaluateArgs(argc, argv, "--simulateSheep", "false");
	String argMowerURL = evaluateArgs(argc, argv, "--mowerURL", "http://localhost/");


	if (argMode == "modeTestCamera")
	{
		//
		// Test camera
		//

		GUI gui;
		VideoCamera videoCamera;
		videoCamera.show(gui);
		videoCamera.test(argCameraURL);

	}
	else if (argMode == "modeTrack")
	{
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

	}
	else if (argMode == "modeAnalyse")
	{
		MainWindow mainWindow;
		mainWindow.start();
	}
	else if (argMode == "modeAnalyseImage")
	{
		//
		// analyse image given by argument argImageName
		//

		GUI gui;
		TrackedObject trackedObject;
		ImageAnalyser imageAnalyser;

		trackedObject.show(gui);
		imageAnalyser.show(gui);
		imageAnalyser.analyse(argImageName, trackedObject);

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
//		int64 frameProcessingStart = getTickCount();// for measuring duration of frame processing loop
		int frametime =  10;	// take one frame each 10ms
//		int framedelay = 10; 	// the delay for next round trip in oder to reach a 'frametime'
								// (must be more than 0 in order to not stop program)
//		int framesTaken = 0;	// counts frames for some actions only have to happen after ...
//		int skipFrames = 10;	// ... skipFrames number of frames were captured

		bool stop(false);		// flag to stop the endless loop
//		int routeIdx = 0;		// index into sequence of rout points, starting with first rout point

		//
		// instantiate classes
		//
		Stopwatch stopwatch;
		GUI gui;
		Garden garden(GARDEN);
		VideoCamera videoCamera;
		TrackedObject trackedObject;
		ImageAnalyser imageAnalyser;
		Planer planer(trackedObject);
		HTTPClient client(argMowerURL);

		gui.printInfo(1, "Hit 'h' for help.");
		gui.printInfo(3, "Initializing...");

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

		// tell planer to where sheep should move first
		planer.setAim(garden.getNextRoutePoint());

		// show them all
		videoCamera.show(gui);
		garden.show(gui);
		imageAnalyser.show(gui);
		planer.show(gui);

		//
		// for ever....
		//
		gui.printInfo(3, "Running...");
		while (!stop) {

			//
			// calc new framedelay
			//
			int64 algtime = stopwatch.getElapsedTime();
			int framedelay = stopwatch.getNextFrameDelay(frametime);

			//
			// wait and get user keyboard input
			//
			int line = 3;
			char key = waitKey(framedelay);
			switch (key) {
			case 'h':
				gui.printInfo(line++, "Help:");
				gui.printInfo(line++, "");
				gui.printInfo(line++, " +     speed up");
				gui.printInfo(line++, " -     speed down");
				gui.printInfo(line++, " 1-9   show windows 1 to 9");
				gui.printInfo(line++, " 0     show all windows");
				gui.printInfo(line++, " <ESC> Exit");
				gui.printInfo(line++, "");
				gui.printInfo(line++, "Hit any key.");
				while (waitKey(100) == -1) {				};
				line = 3;
				gui.printInfo(line++, "Running...");
				gui.printInfo(line++, "");
				gui.printInfo(line++, "");
				gui.printInfo(line++, "");
				gui.printInfo(line++, "");
				gui.printInfo(line++, "");
				gui.printInfo(line++, "");
				gui.printInfo(line++, "");
				gui.printInfo(line++, "");
				break;
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
			case -1: // no key pressed
				break;
			default:
				cout << "key '" << key << "' pressed." << endl;
			}
			if (key >= 48 && key < 58)
			{
				gui.showWindow(key - 48);
			}

			//
			// read next frame if any
			//
			if (!videoCamera.read(frame)) {
				printf("Could not read frame from camera.\n");
				break;
			}

			//
			// process frame only each 100ms
			//
			if (stopwatch.getElapsedTime() < 100) continue;

			//
			// get position of tracked object (sheep)
			//
			garden.setImage(frame);
			Mat greenImage = garden.maskOutGreen(frame);
			bool objectDetected = imageAnalyser.detectObjectPosition(greenImage, trackedObject);
			if (!objectDetected) continue;

			//
			// when aim is reached, get next aim for planer.
			//
			if (planer.isRoutePointReached()) {
				Point2f aim = garden.getNextRoutePoint();
				if (!planer.setAim(aim)) stop = true;
			}

			//
			// calc and issue steering command
			//
			int rotate = planer.plan();
			planer.show(frame);

			//
			// just for simulation: draw sheep in camera frame
			//
			if (argSimulateSheep == "true")
			{
				sheep.update();
				sheep.draw(frame);
				sheep.rotate(rotate);
				sheep.setSpeed(planer.getMotorSpeed1(), planer.getMotorSpeed2());
				sheep.print();
			}

			//
			// process frame only each second
			//
			if (stopwatch.getElapsedTime() < 1000) continue;

			//
			// send command to roboSheep via http
			//
			client.sendMotorSpeeds(planer.getMotorSpeed1(), planer.getMotorSpeed2());
			std::ostringstream oss;
			oss << "HTTPClient::sendMotorSpeeds: " << planer.getMotorSpeed1() << " / " << planer.getMotorSpeed2();
			gui.printInfo(10, oss.str());


			//-------------------------------------------------------------------------
			// Algorithm Done
			//-------------------------------------------------------------------------

			cout << endl << "-----------------------------------------------------" << endl;
			cout << "main:		algtime=" << stopwatch.getElapsedTime() << endl;
			stopwatch.reset();
		}
	}
	return (0);
}


