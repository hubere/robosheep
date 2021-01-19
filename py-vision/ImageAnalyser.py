import cv2
import numpy as np

from TrackedObject import TrackedObject
from Utils import Point
from globals import GUI


def findContours(imageToAnalyse, colorBlob):
    # vector < vector < Point > >
    contour = []
    return contour


def findBestContour(leftContours, param, param1):
    pass


class ImageAnalyser:
    """
    Do some analysis on the camera image
    """

    def __init__(self, frame=None):
        self.frame = frame
        self.dirty = True

    def detect_by_contours(self, frame, trackedObject : TrackedObject):

        imageToAnalyse = frame.copy()

        #int leftBlobIdx = 0;
        #int rightBlobIdx = 0;
        #vector < vector < Point > > leftContours;
        #vector < vector < Point > > rightContours;
        colorBlobs = trackedObject.getColorBlobs()

        #
        # tracked object where we cannot distinguish colorblobs by color range, e.g. light by night
        #
        if colorBlobs.size() == 1:
            colorBlob = colorBlobs[0]
            leftContours = findContours(imageToAnalyse, colorBlob)
            rightContours = leftContours
            if len(leftContours) < 1:
                return False

            if len(leftContours) == 2:
                # TODO something (see cpp)
                print("average0: TODO")

            if len(leftContours) < 1 or len(rightContours) < 1:
                return False

        #
        # tracked Object with two colors
        #
        if colorBlobs.size() == 2:
            colorBlob = colorBlobs[0]
            leftContours = findContours(imageToAnalyse, colorBlob)
            leftBlobIdx = findBestContour(leftContours, colorBlob.getMinSize(), colorBlob.getMaxSize());

            colorBlob = colorBlobs[1]
            rightContours = findContours(imageToAnalyse, colorBlob)
            rightBlobIdx = findBestContour(leftContours, colorBlob.getMinSize(), colorBlob.getMaxSize());


        if len(leftContours) < 1 or len(rightContours) < 1:
            return False

        if leftBlobIdx < 0 or rightBlobIdx < 0:
            return False

    def analyseImage(self, filename: str, trackedObject: TrackedObject):

        """
        see https://github.com/alkasm/colorfilters
        :param filename:
        :param trackedObject:
        :return:
        """
        pass

    def analyse(self, imageToAnalyse, trackedObject: TrackedObject):
        pass

    def detectObjectPositionByMoments(self, frame, trackedObject: TrackedObject):

        #treshColorLow = np.array([20, 100, 100])
        #treshColorHi = np.array([30, 255, 255])
        treshColorLow = np.array([92, 102, 134])
        treshColorHi = np.array([102, 219, 244])

        imgHSV = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
        imgThreshed = cv2.inRange(imgHSV, treshColorLow, treshColorHi)

        GUI.set_frame_aux1(imgHSV)
        GUI.set_frame_aux2(imgThreshed)

        mo = cv2.moments(imgThreshed);

        area = mo['m00']
        mo10 = mo['m10']
        mo01 = mo['m01']

        if area == 0:
            print("could not find color. Maybe we got a grayscale image?!")
            area = 0.1

        GUI.putText("Moments: ", 10)
        GUI.putText(" area=" + str(area), 11)
        GUI.putText(" mo10=" + str(mo10), 12)
        GUI.putText(" mo01=" + str(mo01), 13)
        GUI.putText(" center=(" + str(int(mo10/area)) + "," + str(int(mo01/area)) + ")", 14)

        trackedObject.setAktualPos(Point(int(mo10/area), int(mo01/area)))

        pass

    def detectObjectPositionByContours(self, frame, trackedObject: TrackedObject):

        pass

