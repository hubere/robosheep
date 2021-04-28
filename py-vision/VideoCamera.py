from threading import Thread
from time import sleep

import cv2
import numpy as np

from CountsPerSec import CountsPerSec
from globals import GUI

GARDEN_TRANSFORM_PERSPECTIVE = None

# vlcsnap-2021-03-23-16h41m32s576.png
GARDEN_TRANSFORM_PERSPECTIVE = np.array(eval("[(850, 70), (1300, 600), (1300, 1070), (850, 900)]"), dtype = "float32")
#GARDEN_TRANSFORM_PERSPECTIVE = np.array(eval("[(1120, 0), (1700, 610), (1700, 920), (1100, 750)]"), dtype = "float32")

#GARDEN_TRANSFORM_PERSPECTIVE = np.array(eval("[(672, 0), (1300, 700), (1300, 1030), (500, 700)]"), dtype = "float32")
#GARDEN_TRANSFORM_PERSPECTIVE = np.array(eval("[(0, 688), (1300, 334), (1800, 360), (756, 1012)]"), dtype = "float32")

VIDEO_ROTATE = None
# VIDEO_ROTATE = True


class VideoCamera:
    """
    Class that continuously gets frames from a VideoCapture object
    with a dedicated thread.
    """

    def __init__(self, src=""):
        self.stream = None
        self.frame = None
        if src.startswith("http") or src.startswith("rtsp"):
            self.stream = cv2.VideoCapture(src)
            (self.grabbed, self.frame) = self.stream.read()
        else:
            self.filename = src
        self.stopped = False

    def start(self):
        Thread(target=self.get, args=()).start()
        return self

    def get(self):
        cps = CountsPerSec().start()

        # initialize the FPS throughput estimator
        fps = None

        while not self.stopped:
            if self.stream is not None:
                # read from camera stream
                (self.grabbed, frame) = self.stream.read()
                if self.grabbed:
                    if GARDEN_TRANSFORM_PERSPECTIVE is not None:
                        frame = self.four_point_transform(frame, GARDEN_TRANSFORM_PERSPECTIVE)
                    self.frame = frame
                    cps.increment()
                    GUI.putText("VideoCamera: {:.0f} fps".format(cps.countsPerSec()), 1)

            else:
                # read from file
                frame = cv2.imread(self.filename)
                if frame is not None:
                    # frame = self.dampen_image(frame)
                    if VIDEO_ROTATE is not None:
                        frame = np.rot90(frame, axes=(-3, -2))
                        # frame = frame.swapaxes(-3,-2)[...,::-1,:] # see https://stackoverflow.com/questions/43864855/rotating-images-by-90-degrees-for-a-multidimensional-numpy-array
                    if GARDEN_TRANSFORM_PERSPECTIVE is not None:
                        frame = self.four_point_transform(frame, GARDEN_TRANSFORM_PERSPECTIVE)
                    self.frame = frame

            sleep(0.1)

    def dampen_image(self, frame):
        hsv_img = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
        # decreasing the V channel by a factor from the original
        hsv_img[..., 2] = hsv_img[..., 2] * 0.6
        frame = cv2.cvtColor(hsv_img, cv2.COLOR_HSV2BGR)
        return frame

    def stop(self):
        self.stopped = True

    def order_points(self, pts):
        # initialzie a list of coordinates that will be ordered
        # such that the first entry in the list is the top-left,
        # the second entry is the top-right, the third is the
        # bottom-right, and the fourth is the bottom-left
        rect = np.zeros((4, 2), dtype="float32")
        # the top-left point will have the smallest sum, whereas
        # the bottom-right point will have the largest sum
        s = pts.sum(axis=1)
        rect[0] = pts[np.argmin(s)]
        rect[2] = pts[np.argmax(s)]
        # now, compute the difference between the points, the
        # top-right point will have the smallest difference,
        # whereas the bottom-left will have the largest difference
        diff = np.diff(pts, axis=1)
        rect[1] = pts[np.argmin(diff)]
        rect[3] = pts[np.argmax(diff)]
        # return the ordered coordinates
        return rect

    def four_point_transform(self, image, pts):
        # obtain a consistent order of the points and unpack them
        # individually
        #rect = self.order_points(pts)
        rect = pts
        (tl, tr, br, bl) = rect
        # compute the width of the new image, which will be the
        # maximum distance between bottom-right and bottom-left
        # x-coordiates or the top-right and top-left x-coordinates
        widthA = np.sqrt(((br[0] - bl[0]) ** 2) + ((br[1] - bl[1]) ** 2))
        widthB = np.sqrt(((tr[0] - tl[0]) ** 2) + ((tr[1] - tl[1]) ** 2))
        maxWidth = max(int(widthA), int(widthB))
        # compute the height of the new image, which will be the
        # maximum distance between the top-right and bottom-right
        # y-coordinates or the top-left and bottom-left y-coordinates
        heightA = np.sqrt(((tr[0] - br[0]) ** 2) + ((tr[1] - br[1]) ** 2))
        heightB = np.sqrt(((tl[0] - bl[0]) ** 2) + ((tl[1] - bl[1]) ** 2))
        maxHeight = max(int(heightA), int(heightB))
        # now that we have the dimensions of the new image, construct
        # the set of destination points to obtain a "birds eye view",
        # (i.e. top-down view) of the image, again specifying points
        # in the top-left, top-right, bottom-right, and bottom-left
        # order
        dst = np.array([
            [0, 0],
            [maxWidth - 1, 0],
            [maxWidth - 1, maxHeight - 1],
            [0, maxHeight - 1]], dtype="float32")
        # compute the perspective transform matrix and then apply it
        M = cv2.getPerspectiveTransform(rect, dst)
        warped = cv2.warpPerspective(image, M, (maxWidth, maxHeight))
        # return the warped image
        return warped
