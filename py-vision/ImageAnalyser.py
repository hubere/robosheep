import cv2
import numpy as np
from imutils.video import VideoStream
from imutils.video import FPS, fps
import argparse
import imutils

import Utils
from TrackedObject import TrackedObject
from Utils import Point
from globals import GUI


def findContours(image_to_analyse, color_blob):
    # vector < vector < Point > >
    contour = []
    return contour


def findBestContour(left_contours, param, param1):
    pass


class ImageAnalyser:
    """
    Do some analysis on the camera image
    """

    def __init__(self, algorithm="momements", bgs_algo='MOG2'):
        self.algorithm = algorithm
        self.frame = None
        self.dirty = True

        # create Background Subtractor objects
        if bgs_algo == 'MOG2':
            self.backSub = cv2.createBackgroundSubtractorMOG2()
        else:
            self.backSub = cv2.createBackgroundSubtractorKNN()

        # initialize the bounding box coordinates of the object we are going
        # to track
        self.initBB = None

        # initialize the FPS throughput estimator
        self.fps = None
        self.tracker = None

    def detect(self, frame, tracked_object: TrackedObject):
        if GUI.last_key == ord("h"):
            self.algorithm = "moments"
        if GUI.last_key == ord("j"):
            self.algorithm = "backgroundSubstraction"
        if GUI.last_key == ord("k"):
            self.algorithm = "contours"
        if GUI.last_key == ord("l"):
            self.algorithm = "tracker"

        GUI.putText("ImageAnalyser: %s (h-moments  j-backgroundSubstraction  k-contours  l-tracker)" % self.algorithm, 3)

        if self.algorithm == "backgroundSubstraction":
            self._detectObjectPositionByBackgroundSubstraction(frame, tracked_object)
        elif self.algorithm == "contours":
            self._detectObjectPositionByContours(frame, tracked_object)
        elif self.algorithm == "tracker":
            self._detect_by_tracker(frame, tracked_object)
        else:
            self._detectObjectPositionByMoments(frame, tracked_object)

    def _detectObjectPositionByContours(self, frame, tracked_object: TrackedObject):

        image_to_analyse = frame.copy()

        # int left_blob_idx = 0;
        # int right_blob_idx = 0;
        # vector < vector < Point > > left_contours;
        # vector < vector < Point > > right_contours;
        color_blobs = tracked_object.getColorBlobs()

        #
        # tracked object where we cannot distinguish colorblobs by color range, e.g. light by night
        #
        if len(color_blobs) == 1:
            color_blob = color_blobs[0]
            left_contours = findContours(image_to_analyse, color_blob)
            right_contours = left_contours
            if len(left_contours) < 1:
                return False

            if len(left_contours) == 2:
                # TODO something (see cpp)
                print("average0: TODO")

            if len(left_contours) < 1 or len(right_contours) < 1:
                return False

        #
        # tracked Object with two colors
        #
        if len(color_blobs) == 2:
            color_blob = color_blobs[0]
            left_contours = findContours(image_to_analyse, color_blob)
            left_blob_idx = findBestContour(left_contours, color_blob.getMinSize(), color_blob.getMaxSize());

            color_blob = color_blobs[1]
            right_contours = findContours(image_to_analyse, color_blob)
            right_blob_idx = findBestContour(left_contours, color_blob.getMinSize(), color_blob.getMaxSize());

        if len(left_contours) < 1 or len(right_contours) < 1:
            return False

        if left_blob_idx < 0 or right_blob_idx < 0:
            return False

    def _detectObjectPositionByMoments(self, frame, tracked_object: TrackedObject):
        color_blobs = tracked_object.getColorBlobs()
        if len(color_blobs) == 2:

            # tresh_color_low = np.array([20, 100, 100])
            # tresh_color_hi = np.array([30, 255, 255])
    #        tresh_color_low = np.array([92, 102, 134])
    #        tresh_color_hi = np.array([102, 219, 244])
            color_left = tracked_object.getColorBlobs()[0]
            color_right = tracked_object.getColorBlobs()[1]
            tresh_color_low = tracked_object.getColorBlobs()[0].tresh_low
            tresh_color_hi = tracked_object.getColorBlobs()[0].tresh_hi

            img = frame.copy()
            img_hsv = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
            img_threshed_left = cv2.inRange(img_hsv, color_left.tresh_low, color_left.tresh_hi)
            img_threshed_right = cv2.inRange(img_hsv, color_right.tresh_low, color_right.tresh_hi)

            GUI.set_frame_aux1(img_threshed_left)
            GUI.set_frame_aux2(img_threshed_right)

            mo = cv2.moments(img_threshed_left)

            area = mo['m00']
            mo10 = mo['m10']
            mo01 = mo['m01']

            if area == 0:
                print("could not find color. Maybe we got a grayscale image?!")
                area = 0.1

            position_left = Point(int(mo10 / area), int(mo01 / area))

            mo = cv2.moments(img_threshed_right)

            area = mo['m00']
            mo10 = mo['m10']
            mo01 = mo['m01']

            if area == 0:
                print("could not find color. Maybe we got a grayscale image?!")
                area = 0.1

            position_right = Point(int(mo10 / area), int(mo01 / area))

            GUI.putText(" area=" + str(area), 4)
            GUI.putText(" mo10=" + str(mo10), 5)
            GUI.putText(" mo01=" + str(mo01), 6)
            GUI.putText(" center=(" + str(int(mo10 / area)) + "," + str(int(mo01 / area)) + ")", 14)

            center = Point((position_left.x + position_right.x) / 2, (position_left.y + position_right.y) / 2)
            direction = Utils.getKurswinkelDegree(position_left, position_right) + 90
            if direction > 360:
                direction = direction - 360
            tracked_object.set_position_and_direction(center, direction)
            pass

    def _detectObjectPositionByBackgroundSubstraction(self, frame, tracked_object: TrackedObject):
        """
        taken from https://docs.opencv.org/3.4/d1/dc5/tutorial_background_subtraction.html
        :param frame:
        :param trackedObject:
        :return:
        """

        # update the background model
        fgmask = self.backSub.apply(frame)

        GUI.set_frame_aux1(fgmask)

        (contours, hierarchy) = cv2.findContours(fgmask.copy(), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

        # looping for contours
        for c in contours:
            if cv2.contourArea(c) < 500:
                continue

            # get bounding box from countour
            (x, y, w, h) = cv2.boundingRect(c)

            # draw bounding box
            cv2.rectangle(frame, (x, y), (x + w, y + h), (0, 255, 0), 2)

            tracked_object.set_position_and_direction(Point(int(x+w/2), int(y+h/2)))


#        GUI.putText("Moments: ", 10)
#        GUI.putText(" area=" + str(area), 11)
#        GUI.putText(" mo10=" + str(mo10), 12)
#        GUI.putText(" mo01=" + str(mo01), 13)
#        GUI.putText(" center=(" + str(int(mo10 / area)) + "," + str(int(mo01 / area)) + ")", 14)


        pass

    def _detect_by_tracker(self, frame, traced_object: TrackedObject):
        """
        taken from https://www.pyimagesearch.com/2018/07/30/opencv-object-tracking/

        :param frame:
        :param traced_object:
        :return:
        """
        # tracker_algo = args["tracker"]
        tracker_algo = "mil"

        if self.initBB is None:

            # extract the OpenCV version info
            (major, minor) = cv2.__version__.split(".")[:2]
            # if we are using OpenCV 3.2 OR BEFORE, we can use a special factory
            # function to create our object tracker
            if int(major) == 3 and int(minor) < 3:
                self.tracker = cv2.Tracker_create(tracker_algo.upper())
            # otherwise, for OpenCV 3.3 OR NEWER, we need to explicity call the
            # approrpiate object tracker constructor:
            else:
                # initialize a dictionary that maps strings to their corresponding
                # OpenCV object tracker implementations
                OPENCV_OBJECT_TRACKERS = {
                    "csrt": cv2.TrackerCSRT_create,
                    "kcf": cv2.TrackerKCF_create,
                    # "boosting": cv2.TrackerBoosting_create,
                    "mil": cv2.TrackerMIL_create,
                    # "tld": cv2.TrackerTLD_create,
                    # "medianflow": cv2.TrackerMedianFlow_create,
                    # "mosse": cv2.TrackerMOSSE_create
                }
                # grab the appropriate object tracker using our dictionary of
                # OpenCV object tracker objects
                self.tracker = OPENCV_OBJECT_TRACKERS[tracker_algo]()

            # select the bounding box of the object we want to track (make
            # sure you press ENTER or SPACE after selecting the ROI)
            self.initBB = cv2.selectROI("selectROI", frame, fromCenter=False,
                                   showCrosshair=True)
            cv2.destroyWindow('selectROI')

            # start OpenCV object tracker using the supplied bounding box
            # coordinates, then start the FPS throughput estimator as well
            self.tracker.init(frame, self.initBB)
            self.fps = FPS().start()

        else:
            (H, W) = frame.shape[:2]

            # grab the new bounding box coordinates of the object
            (success, box) = self.tracker.update(frame)

            # check to see if the tracking was a success
            if success:
                (x, y, w, h) = [int(v) for v in box]
                cv2.rectangle(frame, (x, y), (x + w, y + h),
                              (0, 255, 0), 2)

                traced_object.set_position_and_direction(Point(x + w/2, y + h/2))

            # update the FPS counter
            self.fps.update()
            self.fps.stop()

            # initialize the set of information we'll be displaying on
            # the frame
            info = [
                ("Tracker", tracker_algo),
                ("Success", "Yes" if success else "No"),
                ("FPS", "{:.2f}".format(self.fps.fps())),
            ]
            # loop over the info tuples and draw them on our frame
            for (i, (k, v)) in enumerate(info):
                text = "{}: {}".format(k, v)
                cv2.putText(frame, text, (10, H - ((i * 20) + 20)),
                            cv2.FONT_HERSHEY_SIMPLEX, 0.6, (0, 0, 255), 2)


        pass