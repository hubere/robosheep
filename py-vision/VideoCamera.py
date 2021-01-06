from threading import Thread
import cv2

from CountsPerSec import CountsPerSec


class VideoCamera:
    """
    Class that continuously gets frames from a VideoCapture object
    with a dedicated thread.
    """

    def __init__(self, src=0):
        self.stream = cv2.VideoCapture(src)
        (self.grabbed, self.frame) = self.stream.read()
        self.stopped = False

    def start(self):
        Thread(target=self.get, args=()).start()
        return self

    def get(self):
        cps = CountsPerSec().start()
        while not self.stopped:
            if not self.grabbed:
                self.stop()
            else:
                (self.grabbed, self.frame) = self.stream.read()
                cps.increment()
                #frame = putIterationsPerSec(frame, cps.countsPerSec())


    def stop(self):
        self.stopped = True
