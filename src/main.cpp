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

void showGreens(Mat &image, vector<vector<Point> > &contours) {
	for (unsigned i = 1; i < contours.size(); i++) {
		drawContours(image, contours, i, cvScalar(0, 0, 255), 2, 8);
	}
}

void showRoute(Mat &image, vector<vector<Point> > &contours) {
	for (unsigned i = 1; i < contours.size(); i++) {
		drawContours(image, contours, i, cvScalar(0, 255, 0), 1, 8);
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

int main(int argc, char** argv) {

	printf("Working directory (argv[0]): %s\n", argv[0]);

	//
	// evaluate arguments
	//
	string cameraURL = evaluateArgs(argc, argv, "--cameraURL", "http://192.168.1.105/video.cgi?x.mjpg");


	if (argc > 2) {
		std::string arg1 = argv[1];
		std::string arg2 = argv[2];

		if ((arg1 == "-i") || (arg1 == "--image")) {

			//
			// we want to analyse an image
			//

			GUI gui;
			TrackedObject trackedObject;
			ImageAnalyser imageAnalyser;

			imageAnalyser.analyse(arg2, trackedObject);

			exit(0);
		}

	} else if (argc > 1) {
		std::string arg1 = argv[1];

		if (arg1 == "--testHTTP") {

			HTTPClient client;
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

		} else if (arg1 == "--virtualSheep") {

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
		} else if (arg1 == "--analyse") {

			MainWindow mainWindow;
			mainWindow.start();
		}
	} else {

		//
		// control the mower
		//

		//
		// initialize variables
		//
		Mat frame;	// actual image of camera
		Mat mowed;	// actual image of already mowed garden
		//Mat result;	// no idea

		// Each frame should be processes in same amount of time, the 'frametime'. The time
		// consumed by a frame processing round trip is substracted from 'frametime' and stored
		// in 'framedelay'. The next round trip will start by waiting 'framedelay' ms. Hence
		// a frame will be processed each 'frametime' ms.
		int frametime = 1000; 			 	// take one frame each 1000ms
		double frameProcessingStart = 0; 	// for measuring duration of frame processing loop
		int framedelay = 10; 				// the delay for next round trip in oder to reach a 'frametime'
											// (must be more than 0 in order to not stop program)

		bool stop(false);		// flag to stop the endless loop
		int routeIdx = 0;		// index into sequence of rout points, starting with first rout point
		Point_<int> roboPos;	// actual position of sheep

		// These are not used?!
		// RNG rng(12345);
		// int thresh = 100;


		//
		// instantiate classes
		//
		GUI gui;
		Garden garden(2);
		VideoCamera videoCamera;
		TrackedObject trackedObject;
		ImageAnalyser imageAnalyser;
		Planer planer;
		HTTPClient client;

		vector<TrackedColorBlob> colorBlobs = trackedObject.getColorBlobs();
		TrackedColorBlob colorBlobYellow = colorBlobs[0];
		TrackedColorBlob colorBlobRed = colorBlobs[1];

//		planer.show(gui);


		// tell planer to where sheep should move
//		planer.setAim(garden.getRoutePoint(routeIdx));



		//
		// open camera stream
		//
		if (!videoCamera.open(cameraURL)){
			printf("\nCould not open camera '%1$s' \n", cameraURL.c_str());
			videoCamera.probeUrls();
			exit(-1);
		}
		printf("\nOpened camera '%1$s'\n\n", cameraURL.c_str());
		videoCamera.show(gui);

		//
		// initialize images
		//
		if (!videoCamera.read(frame)) {
			return 0;
		}
		frame.copyTo(mowed);
		// frame.copyTo(result);

		showGreens(mowed, garden.getGreenContours());
		showRoute(mowed, garden.getRoutes());
		// showGreens(result, garden.getGreenContours());

		imageAnalyser.show(gui);
		trackedObject.show();

		// TODO FIXME what is this for?
//		imshow("result", result);
//		setMouseCallback("result", mouseHandler);
//
//		createTrackbar(" Canny thresh:", "result", &thresh, 255);

		//
		// for ever....
		//
		while (!stop) {

			//
			// user keyboard control
			//
			char key = cv::waitKey(framedelay);
			switch (key) {
			case 27:
				stop = true;
				break;
//			case 'n':
//				// unmow
//				frame.copyTo(mowed);
//				showGreens(mowed, garden.getGreenContours());
//				showRoute(mowed, garden.getRoutes());
//				break;
//			case 's': // set sheep to start position
//				sheep.setPosition(Point2f(250, 300));
//				break;
//			case '+': // speed up sheep
//				sheep.speedUp();
//				break;
//			case '-': // slow down sheep
//				sheep.slowDown();
//				break;
			}

			// measure processing time
			frameProcessingStart = (double)getTickCount();

			// read next frame if any
			if (!videoCamera.read(frame)) {
				printf("Could not read frame from camera.\n");
				break;
			}

			// just for simulation: draw shepp in camera frame
			// sheep.update();
			// sheep.draw(frame);

			//
			// get position and direction of tracked object from imageAnalyser
			//
			Point2f centerYellow = imageAnalyser.detectObjectPosition(frame, colorBlobYellow);
			Point2f centerRed = imageAnalyser.detectObjectPosition(frame, colorBlobRed);
			Point2f center = (centerYellow + centerRed) * .5;  // in the middle of the colorBlobCenters
			if (center.x == 0 && center.y == 0)
			{
				printf("Could calculate center.\n");
				continue;
			}

			//
			// calculate direction
			//
			Point2f v;
			v.x = centerRed.x - centerYellow.x;
			v.y = centerRed.y - centerYellow.y;
			// rotate vector 90 degrees
			float temp = v.y;
			v.y = -v.x;
			v.x = temp;

			//
			// get position of tracked object
			//
			bool objectDetected = imageAnalyser.detectObjectPosition(frame, trackedObject);
			if (!objectDetected) continue;
			Point_<int> roboPos = trackedObject.getAktualPos();
			//  TODO FIXME merge: Point_<int> lastPos = trackedObject.getLastPos();

			//
			// update mowed image
			//
//  TODO FIXME merge:
//			if (lastPos.x > 0 && lastPos.y > 0 && roboPos.x > 0
//					&& roboPos.y > 0) {
//				line(mowed, roboPos, lastPos, cvScalar(255, 255, 255), 10);
//			}

			//
			// get next aim
			//
			if (planer.isRoutePointReached(roboPos)) {
				routeIdx++;
				if (routeIdx >= (int) garden.getRouteSize())
					stop = true;
				Point2f aim = garden.getRoutePoint(routeIdx);
				planer.setAim(aim);
			}

			//
			// calc and issue steering command
			//
			planer.setAktualPosition(roboPos);
			//  TODO FIXME merge: int rotate = planer.plan(lastPos, roboPos);

			// sheep.rotate(rotate);
			// sheep.setSpeed(planer.getMotorSpeed1(), planer.getMotorSpeed2());
			// sheep.print();

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
//			char command[255];
//			sprintf(command, "motor?m1=%d&m2=%d", planer.getMotorSpeed1(), planer.getMotorSpeed2());
//			client.sendMessage(command);


			//-------------------------------------------------------------------------
			// Algorithm Done
			//-------------------------------------------------------------------------

			// show foreground
			imshow("mowed", mowed);

			// calc next framedelay
			int algtime = ((double)getTickCount() - frameProcessingStart)*1000./getTickFrequency();
			if (frametime > algtime)
				framedelay = frametime - algtime;
			else
				framedelay = 10;
			printf(" algtime=%d framedelay=%d\n", algtime, framedelay);

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

