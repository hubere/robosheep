import cv2
import numpy as np

from TrackedObject import TrackedObject
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

        moments = cv2.moments(imgThreshed);

        print("done")
#        trackedObject.setAktualPos(Point_ < int > (int(floor(moment10 / area)), int(floor(moment01 / area))));

        pass

    def detectObjectPositionByContours(self, frame, trackedObject: TrackedObject):

        pass

