from threading import Thread
from time import sleep

import cv2

from CountsPerSec import CountsPerSec
from globals import GUI


class VideoCamera:
    """
    Class that continuously gets frames from a VideoCapture object
    with a dedicated thread.
    """

    def __init__(self, src=0):
        self.src = src
        self.stream = cv2.VideoCapture(src)
        (self.grabbed, self.frame) = self.stream.read()
        self.stopped = False

    def start(self):
        Thread(target=self.get, args=()).start()
        return self

    def get(self):
        cps = CountsPerSec().start()

        # initialize the FPS throughput estimator
        fps = None

        while not self.stopped:
            (self.grabbed, frame) = self.stream.read()
            if not self.grabbed:
                # no frame could be read from camera ... try to read from file
                frame = cv2.imread(self.src)
                #dampen image
                hsvImg = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
                # decreasing the V channel by a factor from the original
                hsvImg[..., 2] = hsvImg[..., 2] * 0.6
                frame = cv2.cvtColor(hsvImg, cv2.COLOR_HSV2BGR)
                self.frame = frame
                sleep(0.1)

            else:
                self.frame = frame
                cps.increment()
                GUI.putText("VideoCamera: {:.0f} fps".format(cps.countsPerSec()), 1)

    def stop(self):
        self.stopped = True
