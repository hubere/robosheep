from threading import Thread
import cv2

from CountsPerSec import CountsPerSec
from globals import GUI


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
            (self.grabbed, self.frame) = self.stream.read()
            if not self.grabbed:
                self.stop()
            else:
                cps.increment()
                GUI.putText("VideoCamera: {:.0f} fps".format(cps.countsPerSec()), 1)

    def stop(self):
        self.stopped = True
