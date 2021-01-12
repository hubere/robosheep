import cv2

from TrackedObject import TrackedObject


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

    def analyse(self, imageToAnalyse, trackedObject):
        pass

