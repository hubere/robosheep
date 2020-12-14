/*
* This is the main programm of robosheep. Basically it evaluates calling 
* parameters and starts the application. Programm arguments are:
* --cameraURL		that's where the camera can be reached
* --mowerURL		that's where the mover can be controlled 
* --mode			one of the operating modes: 
*						modeTestCamera:		just to test vision system
*						modeAnalyseImage:	analyse a given image
*						modeTrack:			just track an object on camera
*					if no mode is specified, the mover will be controlled.
*
* --image			for debugging purpose, an image that wants to be analysed
* --simulateSheep	for testing, starts a virtual sheep that prints itself 
*					onto the camera image 
*
*  Created on: Nov 10, 2016
*      Author: Edwin Huber
*/

#include "main.h"

#include <stdio.h>
#include <time.h>
#include <math.h>
#include <limits>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/video/tracking.hpp>

#include "presentation/GUI.h"
#include "utilities/OpenCVUtils.h"
#include "utilities/Stopwatch.h"
#include "utilities/VirtualSheep.h"
#include "sensors/VideoCamera.h"
#include "sensors/ImageAnalyser.h"
#include "planning/Garden.h"
#include "planning/Planer.h"
#include "actuators/HTTPClient.h"


using namespace std;
using namespace cv;
using namespace robosheep;

/*
* print calling arguments
*/
void printCallingParameter(int argc, char** argv) {
	cout << "main: === started robosheep ===" << endl;
	for (int i = 0; i < argc; i++)
		cout << "main: arg[" << i << "] " << argv[i] << endl;
	cout << endl;
}

/*
* Evaluate calling parameters
*/
string evaluateArgs(int argc, char** argv, string tag, string defaultValue) {
	for (int i=0; i< argc; i++)
	{
		if (argv[i] == tag)
			return argv[i+1];
	}
	return defaultValue;
}

/*
* Print usage, i.e. key bindings.
*/
void printHelp(GUI& gui)
{
	int line = 3;
	gui.printInfo(line++, "Help:");
	gui.printInfo(line++, "");
	gui.printInfo(line++, " +     speed up");
	gui.printInfo(line++, " -     speed down");
	gui.printInfo(line++, " 1-9   show windows 1 to 9");
	gui.printInfo(line++, " 0     show all windows");
	gui.printInfo(line++, " Right-click in Planer windows to set new aim");
	gui.printInfo(line++, " <ESC> Exit");
	gui.printInfo(line++, "");
	gui.printInfo(line++, "Hit any key.");
	while (waitKey(100) == -1) {};
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
}

/**
* The main method. That's where every thing starts.
*/
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
	String argImageName = evaluateArgs(argc, argv, "--image", "snapshot");
	String argSimulateSheep = evaluateArgs(argc, argv, "--simulateSheep", "false");
	String argMowerURL = evaluateArgs(argc, argv, "--mowerURL", "http://192.168.1.108/");

	if (argMode == "modeTestCamera")
	{
		// --------------------------------------------
		// Test camera
		// --------------------------------------------

		GUI gui;
		VideoCamera videoCamera;
		videoCamera.show(gui);
		videoCamera.test(argCameraURL);
	}
	else if (argMode == "modeTrack")
	{
		// --------------------------------------------
		// we want to track an object
		// --------------------------------------------

		VideoCamera videoCamera;
		TrackedObject trackedObject;
		ImageAnalyser imageAnalyser;

		Mat frame;
		int framedelay = 1000;

		while (videoCamera.read(frame, framedelay)) {
			imageAnalyser.detectObjectPosition(frame, trackedObject);
		}

	}
	else if (argMode == "modeAnalyseImage")
	{
		// --------------------------------------------
		// analyse image given by argument argImageName
		// --------------------------------------------

		GUI gui;
		TrackedObject trackedObject;
		ImageAnalyser imageAnalyser;

		trackedObject.show(gui);
		imageAnalyser.show(gui);
		imageAnalyser.analyse(argImageName, trackedObject);

	}
	else
	{
		// --------------------------------------------
		// control the mower
		// --------------------------------------------

		//
		// initialize variables
		//
		Mat frame;				// actual image of camera

		// Each frame should be processed in same amount of time, the 'frametime'. The time
		// consumed by a frame processing round trip is substracted from 'frametime' and stored
		// in 'framedelay'. The next round trip will start by waiting 'framedelay' ms. Hence
		// a frame will be processed each 'frametime' ms.
		int frametime =  10000;	// take one frame each frametime milliseconds
		int framedelay = 100;	// delay for algorithm to wait in order to reach a algorithm time of frametime
		bool stop(false);		// flag to stop the endless loop
		
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
		VirtualSheep sheep; // our virtual sheep
		OpenCVUtils util;

		gui.printInfo(1, "Hit 'h' for help.");
		gui.printInfo(3, "Initializing...");

		//
		// initialize camera
		// open camera stream or load image
		//
		if (argCameraURL != "noCameraURL"){
			if (!videoCamera.open(argCameraURL))
			{
				waitKey(10000);
				exit(-1);
			}
		}
		else{
			videoCamera.loadImage(argImageName);
		}
		if (!videoCamera.read(frame)) {
			waitKey(5000);
			exit(-1);
		}

		// tell planer to where sheep should move first
		planer.setAim(garden.getNextRoutePoint());

		// show them all
		videoCamera.show(gui);
		garden.show(gui);
		imageAnalyser.show(gui);
		planer.show(gui);

		//
		// for log-video
		//
		Size frameSize = frame.size();
		Mat videoImage(frameSize.height, frameSize.width * 3, CV_8UC3);

		//
		// for ever....
		//
		gui.printInfo(3, "Running...");
		while (!stop) {

			//
			// wait and get user keyboard input
			//
			char key = waitKey(framedelay);
			switch (key) {
			case 'h':
				printHelp(gui);
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
				gui.showWindow(key - 48);

			//
			// read next frame if any
			//
			if (!videoCamera.read(frame)) {
				cout << "Could not read frame from camera." << endl;
				gui.printInfo(5, "Could not read frame from camera.");
				break;
			}
			ostringstream timingInfo;
			timingInfo << "timingInfo:    read frame: " << stopwatch.getElapsedTime();

			//
			// write log-video
			//
			Mat info = gui.getInfoImage();
			Mat anal = imageAnalyser.getAnalysedImage();
			Mat plan = planer.getPlannedImage();
			Mat vi = Mat::zeros(frameSize.height, frameSize.width * 2, CV_8UC3);
			if (anal.empty()) anal = Mat::zeros(480, 640, CV_8UC3);
			if (plan.empty()) plan = Mat::zeros(480, 640, CV_8UC3);
			hconcat(info, anal, vi);
			hconcat(vi, plan, videoImage);
			videoCamera.write(videoImage);
			timingInfo << "; write video: " << stopwatch.getElapsedTime();

			//
			// get position of tracked object (sheep)
			//
			garden.setImage(frame);
			Mat greenImage = garden.maskOutGreen(frame);
			bool objectDetected = imageAnalyser.detectObjectPosition(greenImage, trackedObject);
			timingInfo << "; detect object: " << stopwatch.getElapsedTime();

			if (!objectDetected) {
				gui.printInfo(12, "No object detected");
			}
			else
			{
				// object was detected

				//
				// when aim is reached, tell planer the next aim.
				//
				if (planer.isRoutePointReached()) {
					Point2f aim = garden.getNextRoutePoint();
					if (!planer.setAim(aim)) stop = true;
				}

				//
				// plan and issue steering command
				//
				int rotate = planer.plan();
				planer.show(frame);
				timingInfo << "; plan: " << stopwatch.getElapsedTime();

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
				// send command to roboSheep via http
				//
				client.sendMotorSpeeds(planer.getMotorSpeed1(), planer.getMotorSpeed2());
				std::ostringstream oss;
				oss << "HTTPClient::sendMotorSpeeds: " << planer.getMotorSpeed1() << " / " << planer.getMotorSpeed2();
				gui.printInfo(12, oss.str());
				timingInfo << "; send command: " << stopwatch.getElapsedTime();

			} // END object was detected

			//-------------------------------------------------------------------------
			// Algorithm Done
			//-------------------------------------------------------------------------

			//
			// calc new framedelay
			//
			int algtime = stopwatch.getElapsedTime();
			int framedelay = stopwatch.getNextFrameDelay(frametime);

			//
			// print info
			//
			gui.printInfo(6, "FPMS:            ", videoCamera.getFPMS());
			gui.printInfo(7, "algtime[ms]:     ", (int)algtime);
			gui.printInfo(8, "frameDelay[ms]:  ", framedelay);
			gui.printInfo(9, timingInfo.str());


			cout << endl << "-----------------------------------------------------" << endl;
			cout << "main:		algtime[ms]=" << stopwatch.getElapsedTime() << endl;
			stopwatch.reset();
		}

		gui.printInfo(10, "Hit any key.");
		waitKey(5000);
	}

	return (0);
}



