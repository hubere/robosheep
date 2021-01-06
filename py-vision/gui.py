from threading import Thread

import cv2

from CountsPerSec import CountsPerSec


class Gui:
    """
    Class is responsible for displaying.
    """

    def __init__(self, frame=None):
        self.frame = frame
        self.stopped = False
        w, h = 2, 2
        self.frames = [[0 for x in range(w)] for y in range(h)]

    def show(self):
        # orig 1920 x 1080
        scaled_frame = cv2.resize(self.frame, (960, 540))
        cv2.imshow("Video", scaled_frame)

    def set_frame(self, frame, line: int, column: int):
        self.frames[line][column] = frame
        self.frame = frame
        self.show()

