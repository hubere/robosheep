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

#include "OpenCVUtils.h"
#include "virtualSheep.h"

using namespace std;
using namespace cv;

// our virtual sheep
virtualSheep sheep;
OpenCVUtils util;

static int idx = 0;
static double dist2aim = std::numeric_limits<double>::max();

RNG rng(12345);

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

Point_<int> detectSheepPosition(Mat &frame) {

	/// Convert image to gray and blur it
	Mat src_gray;
	cvtColor(frame, src_gray, CV_BGR2GRAY);
	blur(src_gray, src_gray, Size(3, 3));

	/// Detect edges using Threshold
	Mat threshold_output;
	int thresh = 200;
	int max_thresh = 255;
	threshold(src_gray, threshold_output, thresh, 255, THRESH_BINARY);

	Mat imgHSV;
	Mat imgThreshed;
	Scalar treshColorLow(0, 200, 200);
	Scalar treshColorHi(40, 255, 255);

	// change to HSV color space
	cvtColor(frame, imgHSV, CV_BGR2HSV);

	// treshhold
	inRange(imgHSV, treshColorLow, treshColorHi, imgThreshed);

	namedWindow("contours", 1);

	vector<vector<Point> > contours0;
	vector<Vec4i> hierarchy;
	findContours(imgThreshed, contours0, hierarchy, RETR_TREE,
			CHAIN_APPROX_SIMPLE);
	if (contours0.size() != 1) {
		printf(
				"ERROR! found more or less than one contour! contours count: %i\n",
				contours0.size());
	}

	/// Approximate contours to polygons + get bounding rects and circles
	vector<vector<Point> > contours(contours0.size());
	vector<Rect> boundRect(contours.size());
	vector<Point2f> center(contours.size());
	vector<float> radius(contours.size());
	int rightCountourIdx = -1;
	Size sheepSize = sheep.getSize();
	float minRadius = fmin(sheepSize.width, sheepSize.height) / 2.0;
	float maxRadius = fmax(sheepSize.width, sheepSize.height) * 2.0;
	for (int i = 0; i < contours.size(); i++) {
		approxPolyDP(Mat(contours0[i]), contours[i], 3, true);
		boundRect[i] = boundingRect(Mat(contours[i]));
		minEnclosingCircle(contours[i], center[i], radius[i]);
		if (radius[i] > minRadius and radius[i] < maxRadius)
			rightCountourIdx = i;
	}

	Mat cnt_img;
	frame.copyTo(cnt_img);

	int _levels = 3;
	Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255),
			rng.uniform(0, 255));
	for (uint i = 0; i < contours.size(); i++) {
		drawContours(cnt_img, contours, i, Scalar(128, 255, 255), 3, CV_AA,
				hierarchy, std::abs(_levels));
		circle(cnt_img, center[i], (int) radius[i], color, 2, 8, 0);
	}

	if (rightCountourIdx == -1) {
		return Point(0, 0);
	}

	circle(cnt_img, center[rightCountourIdx], 10, color, 4, 8, 0);

	imshow("contours", cnt_img);

	return center[rightCountourIdx];
}

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

	//
	// global variables
	//

	util.printWorkingDirectory();

	VideoCapture cap = util.openVideoCapture();
	cv::Mat frame;
	cv::Mat mowed;
	cv::Mat result;

	// the detected sheep position
	Point_<int> lastRoboPos;
	Point_<int> roboPos;

	bool stop(false);
	int frametime = 0;
	int framedelay = 0;
	int framecount = 0;
	int algtime = 0;
	int thresh = 100;
	int routeIdx = 0;

	RNG rng(12345);

	//
	// get contours for greens
	//
	// Create a sequence of points to make a contour:
	vector<Point> green(6);
	green[0] = Point(104, 221);
	green[1] = Point(119, 403);
	green[2] = Point(345, 404);
	green[3] = Point(339, 323);
	green[4] = Point(498, 321);
	green[5] = Point(464, 228);

	vector<vector<Point> > greenContours(1);
	greenContours[0] = green;

	//
	// get route
	//
	vector<Point> route(10);
	route[0] = Point(129, 391);
	route[1] = Point(118, 231);
	route[2] = Point(134, 232);
	route[3] = Point(142, 394);
	route[4] = Point(159, 395);
	route[5] = Point(142, 232);
	route[6] = Point(157, 232);
	route[7] = Point(172, 395);
	route[8] = Point(188, 396);
	route[9] = Point(170, 232);

	vector<vector<Point> > routes(1);
	routes[0] = route;

	Point_<int> aim = route[routeIdx];

	//
	// initialize images
	//
	if (!cap.read(frame)) {
		return 0;
	}
	frame.copyTo(mowed);
	frame.copyTo(result);

	showGreens(mowed, greenContours);
	showGreens(result, greenContours);
	showRoute(mowed, routes);

	imshow("result", result);
	setMouseCallback("result", mouseHandler);

	createTrackbar(" Canny thresh:", "result", &thresh, 255);

	//
	// for all frames in video
	//
	while (!stop) {

		// read next frame if any
		if (!cap.read(frame)) {
			break;
		}
		framecount++;

		// just for simulation: draw shepp in camera frame
		sheep.update();
		sheep.draw(frame);

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
		// get sheep position
		//
		lastRoboPos = roboPos;
		roboPos = detectSheepPosition(frame);

		printf(" last=(%d,%d)", lastRoboPos.x, lastRoboPos.y);
		printf(" detected=(%d,%d) error=(%.0f,%.0f)", roboPos.x, roboPos.y,
				sheep.getPosition().x - roboPos.x,
				sheep.getPosition().y - roboPos.y);
		roboPos = sheep.getPosition(); // nur zum debuggen

		//
		// update mowed image
		//
		if (lastRoboPos.x > 0 && lastRoboPos.y > 0 && roboPos.x > 0
				&& roboPos.y > 0) {
			line(mowed, roboPos, lastRoboPos, cvScalar(255, 255, 255), 10);
		}

		//
		// get next aim
		//
		if (isRoutePointReached(aim, roboPos)) {
			routeIdx++;
			if (routeIdx >= (int) route.size())
				stop = true;
			aim = route[routeIdx];
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
		int tiDegree = util.getKurswinkelDegree(lastRoboPos, roboPos); // Kurswinkel ist
		int tsDegree = util.getKurswinkelDegree(roboPos, aim); // Kurswinkel soll

		// 2. calc movement commands
		int rotate = (tsDegree - tiDegree);
		if (rotate > 180)
			rotate = rotate - 360;
		if (rotate < -180)
			rotate = 360 + rotate;
		sheep.rotate(rotate);

		// sheep.rotate(tdDegree);

		printf(" aim=(%d,%d) ti=%d ts=%d => rotate:%d ", aim.x, aim.y, tiDegree,
				tsDegree, rotate);
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
			showGreens(mowed, greenContours);
			showRoute(mowed, routes);
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

	return (0);
}

