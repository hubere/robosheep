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

#include "MainWindow.h"
#include "OpenCVUtils.h"
#include "ImageAnalyser.h"
#include "VideoCamera.h"
#include "Garden.h"
#include "GUI.h"
#include "VirtualSheep.h"
#include "Planer.h"

using namespace std;
using namespace cv;
using namespace robosheep;

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
	for (unsigned i = 0; i < contours.size(); i++) {
		drawContours(image, contours, i, cvScalar(0, 0, 255), 2, 8);
	}
}

void showRoute(Mat &image, vector<vector<Point> > &contours) {
	for (unsigned i = 0; i < contours.size(); i++) {
		drawContours(image, contours, i, cvScalar(0, 255, 0), 1, 8);
	}
}





int main(int argc, char** argv) {

	printf("Working directory (argv[0]): %s\n", argv[0]);
	util.printWorkingDirectory();

	std::string imageName;

	//
	// evaluate arguments
	//
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

			trackedObject.setGimpColor(
					util.openCV2gimpValue(virtualSheep.getColor()));

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
	} else {

		//
		// control the mower
		//

		GUI gui;
		Garden garden;
		VideoCamera videoCamera;
		TrackedObject trackedObject;
		ImageAnalyser imageAnalyser;
		Planer planer;


		imageAnalyser.show(gui);
		// planer.show(gui);


		Mat frame;
		Mat mowed;
		Mat result;

		bool stop(false);
		int frametime = 0;
		int framedelay = 0;
		int framecount = 0;
		int algtime = 0;
		int thresh = 100;
		int routeIdx = 0;

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
		// for all frames in video
		//
		while (!stop) {

			// read next frame if any
			if (!videoCamera.read(frame)) {
				break;
			}
			framecount++;

			// just for simulation: draw shepp in camera frame
			sheep.update();
			//	sheep.draw(frame);

			//-------------------------------------------------------------------------
			// Insert Algorithm here
			//-------------------------------------------------------------------------

			//
			// only each X th frame:
			//
			if (framecount < 10)
				continue;
			framecount = 0;

			//
			// get position of tracked object
			//
			imageAnalyser.detectObjectPosition(frame, trackedObject);
			Point_<int> roboPos = trackedObject.getAktualPos();
			Point_<int> lastPos = trackedObject.getLastPos();

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
				circle(mowed, aim, 5, cvScalar(0, 255, 255), 1);
			}

			//
			// calc and issue steering command
			//
			int rotate = planer.plan(lastPos, roboPos);
			sheep.rotate(rotate);
			sheep.print();
			// TODO FIXME HU also calc speed.
			//moveRobo();

			//-------------------------------------------------------------------------
			// Algorithm Done
			//-------------------------------------------------------------------------

			// show foreground
			imshow("video", frame);
			imshow("mowed", mowed);

			// introduce a delay
			// or press key to stop
			//printf(" frametime = %d", frametime, algtime);
			if (frametime > algtime)
				framedelay = frametime - algtime;
			else
				framedelay = 10;
			// printf(" framedelay = %d\n", framedelay);

			framedelay = 1000;

			//
			// user keyboard control
			//
			char key = cv::waitKey(framedelay);
			// printf(" key = %c\n", key);
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
			case 'i':
				// TODO
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

