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

using namespace std;
using namespace cv;
using namespace robosheep;

// our virtual sheep
VirtualSheep sheep;
OpenCVUtils util;

static int idx = 0;
static double dist2aim = std::numeric_limits<double>::max();

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

Point_<int> detectSheepPosition2(Mat &frame) {
	Mat imgHSV;
	Mat imgThreshed;
	Scalar treshColorLow(20, 100, 100);
	Scalar treshColorHi(30, 255, 255);

	// change to HSV color space
	cvtColor(frame, imgHSV, CV_BGR2HSV);

	// treshhold
	inRange(imgHSV, treshColorLow, treshColorHi, imgThreshed);

	// Calculate the moments to estimate sheep position
	Moments mu;
	mu = moments(imgThreshed, false);

	// The actual moment values
	double moment10 = mu.m10;
	double moment01 = mu.m01;
	double area = mu.m00; // cvGetCentralMoment(moments, 0, 0);

	return Point_<int>(moment10 / area, moment01 / area);
}

const int w = 500;

bool useDistance = false;

bool isRoutePointReached(Point_<int> aim, Point_<int> pos) {
	// using proximity
	int proximity = 5;
	if (aim.x > pos.x - proximity and aim.x < pos.x + proximity
			and aim.y > pos.y - proximity and aim.y < pos.y + proximity) {
		// we are in proximity

		if (!useDistance) {
			return true;
		} else {

			// using min distance
			double dist = sqrt(
					exp(fabs(pos.x - aim.x)) + exp(fabs(pos.y - aim.y)));
			printf("dist2aim: %.0f\n", dist);
			if (dist > dist2aim) {
				return true;
			}
			dist2aim = dist;
		}
	}
	return false;
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

		Garden garden;
		VideoCamera videoCamera;
		TrackedObject trackedObject;
		ImageAnalyser imageAnalyser;

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

		Point_<int> aim = garden.getRoutePoint(routeIdx);

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
			if (isRoutePointReached(aim, roboPos)) {
				routeIdx++;
				if (routeIdx >= (int) garden.getRouteSize())
					stop = true;
				aim = garden.getRoutePoint(routeIdx);
				dist2aim = std::numeric_limits<double>::max();
				circle(mowed, aim, 5, cvScalar(0, 255, 255), 1);
				printf("\nnew aim: (%i,%i)\n", aim.x, aim.y);

			}

			//
			// calc steering command
			//
			// double dist = pointPolygonTest(green, roboPos, 1);
			// printf(" dist = %f\n", dist);
			// if (dist < 0){
			// turn
			//}

			// 1. calc direction to head
			int tiDegree = util.getKurswinkelDegree(lastPos, roboPos); // Kurswinkel ist
			int tsDegree = util.getKurswinkelDegree(roboPos, aim); // Kurswinkel soll

			// 2. calc movement commands
			int rotate = (tsDegree - tiDegree);
			if (rotate > 180)
				rotate = rotate - 360;
			if (rotate < -180)
				rotate = 360 + rotate;
			sheep.rotate(rotate);

			// sheep.rotate(tdDegree);

			printf(" aim=(%d,%d) ti=%d ts=%d => rotate:%d ", aim.x, aim.y,
					tiDegree, tsDegree, rotate);
			sheep.print();

			//		cvtColor(frame, src_gray, CV_BGR2GRAY);
			//		blur(src_gray, src_gray, Size(3, 3));

			//		/// Detect edges using canny
			//		Canny(src_gray, result, thresh, thresh * 2, 3);
			//		/// Find contours
			//		findContours(result, contours, hierarchy, CV_RETR_TREE,
			//				CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

			//		imshow("result", drawing);

			//getCommand(frame);

			//
			// issue steering command
			//
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

