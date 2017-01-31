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
	return "";
}

int main(int argc, char** argv) {

	printf("Working directory (argv[0]): %s\n", argv[0]);

	string imageName;

	//
	// evaluate arguments
	//
	string cameraURL = evaluateArgs(argc, argv, "--cameraURL", "http://192.168.0.115/video.cgi");


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

			namedWindow("video", 1);

			while (videoCamera.read(frame, framedelay)) {
				imageAnalyser.detectObjectPosition(frame, trackedObject);
				imshow("video", frame);
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

		GUI gui;
		Garden garden(2);
		VideoCamera videoCamera;
		TrackedObject trackedObject;
		ImageAnalyser imageAnalyser;
		Planer planer;
		HTTPClient client;

		videoCamera.show(gui);
		imageAnalyser.show(gui);
		trackedObject.show();
		planer.show(gui);


		Mat frame;
		Mat mowed;
		Mat result;

		bool stop(false);
		int frametime = 1000; // take one frame each 1000ms
		double frameProcessingStart = 0;
		int algtime = 0;

		int framedelay = 10; // must be more than 0 in order to not stop program
		int thresh = 100;
		int routeIdx = 0;

		Point_<int> roboPos;

		RNG rng(12345);

		// tell planer to where sheep should move
		planer.setAim(garden.getRoutePoint(routeIdx));

		//
		// initialize images
		//
		if (!videoCamera.read(frame)) {
			return 0;
		}
		frame.copyTo(mowed);
		frame.copyTo(result);

		showGreens(mowed, garden.getGreenContours());
		showGreens(result, garden.getGreenContours());
		showRoute(mowed, garden.getRoutes());

		imshow("result", result);
		setMouseCallback("result", mouseHandler);

		createTrackbar(" Canny thresh:", "result", &thresh, 255);

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
			case 'n':
				// unmow
				frame.copyTo(mowed);
				showGreens(mowed, garden.getGreenContours());
				showRoute(mowed, garden.getRoutes());
				break;
			case 's': // set sheep to start position
				sheep.setPosition(Point2f(250, 300));
				break;
			case '+': // speed up sheep
				sheep.speedUp();
				break;
			case '-': // slow down sheep
				sheep.slowDown();
				break;
			}

			// measure processing time
			frameProcessingStart = (double)getTickCount();

			// read next frame if any
			if (!videoCamera.read(frame)) {
				break;
			}

			// just for simulation: draw shepp in camera frame
			// sheep.update();
			// sheep.draw(frame);

			//
			// get position of tracked object
			//
			if (!imageAnalyser.detectObjectPosition(frame, trackedObject))
				continue;
			Point_<int> lastPos = roboPos;
			roboPos = trackedObject.getAktualPos();

			if (planer.getAim().x == 0)
				continue;

			//
			// update mowed image
			//
			if (lastPos.x > 0 && lastPos.y > 0 && roboPos.x > 0
					&& roboPos.y > 0) {
				line(mowed, roboPos, lastPos, cvScalar(255, 255, 255), 10);
			}

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
			int rotate = planer.plan(lastPos, roboPos);

			// sheep.rotate(rotate);
			// sheep.setSpeed(planer.getMotorSpeed1(), planer.getMotorSpeed2());
			// sheep.print();

			// draw direction indicator
			int length = 50;
			Point P2;
			P2.x =  (int)round(roboPos.x + length * cos(rotate * CV_PI / 180.0));
			P2.y =  (int)round(roboPos.y + length * sin(rotate * CV_PI / 180.0));
			Point2f aim = garden.getRoutePoint(routeIdx);
			circle(frame, aim, 5, cvScalar(0, 255, 255), 1);
			circle(frame, roboPos, 5, cvScalar(0, 255, 0), 1);
			line(frame, roboPos, P2, cvScalar(255, 0, 255), 5);
			planer.show(frame);

			char command[255];
			// sprintf(command, "motor?m1=%d&m2=%d", planer.getMotorSpeed1(), planer.getMotorSpeed2());
			// client.sendMessage(command);


			//-------------------------------------------------------------------------
			// Algorithm Done
			//-------------------------------------------------------------------------

			// show foreground
			imshow("video", frame);
			imshow("mowed", mowed);

			// calc next framedelay
			algtime = ((double)getTickCount() - frameProcessingStart)*1000./getTickFrequency();
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

