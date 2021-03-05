from threading import Thread
from time import sleep

import cv2
import numpy as np
from Utils import Point


# orig 1920 x 1080
IMAGE_HIGHT = 1080
IMAGE_WIDTH = 1920
SCALE = 4

HIGHT = int(IMAGE_HIGHT / SCALE)
WIDTH = int(IMAGE_WIDTH / SCALE)


class Gui:
    """
    Class is responsible for displaying.
    """

    def __init__(self, frame=None):
        self.screen = np.zeros((HIGHT, WIDTH, 3), np.uint8)
        self.frame_info = np.zeros((HIGHT, WIDTH, 3), np.uint8)
        self.frame_video = np.zeros((HIGHT, WIDTH, 3), np.uint8)
        self.frame_aux1 = np.zeros((HIGHT, WIDTH, 3), np.uint8)
        self.frame_aux2 = np.zeros((HIGHT, WIDTH, 3), np.uint8)
        self.dirty = True
        w, h = 2, 2
        self.info_lines = ["" for x in range(26)]
        self.last_click = None
        self.last_key = None
        cv2.namedWindow("Robosheep")
        cv2.setMouseCallback("Robosheep", self.click)

    def update(self):
        cv2.imshow("Robosheep", self.get_screen())
        self.last_key = cv2.waitKey(100)
        if self.last_key == ord("q"):
            exit()
        self.putText("Hit q to quit.    last_key=%s  last_click: %s" % (self.last_key, self.last_click), 25)

    def click(self, event, x, y, flags, param):
        # if the left mouse button was clicked, record the starting
        # (x, y) coordinates
        if event == cv2.EVENT_MBUTTONDOWN:
            self.last_click = Point(x * SCALE, (y-HIGHT) * SCALE)

    def get_screen(self):
        if self.dirty:
            self.frame_info = np.zeros((HIGHT, WIDTH, 3), np.uint8)
            for num, info in enumerate(self.info_lines, start=1):
                cv2.putText(self.frame_info, info, (10, num * 10), cv2.FONT_HERSHEY_SIMPLEX, 0.3, (255, 255, 255))

            scaled_video_frame = cv2.resize(self.frame_video, (WIDTH, HIGHT))
            scaled_frame_aux1 = cv2.resize(self.frame_aux1, (WIDTH, HIGHT))
            scaled_frame_aux2 = cv2.resize(self.frame_aux2, (WIDTH, HIGHT))

            if len(scaled_frame_aux1.shape) == 2:
                scaled_frame_aux1 = cv2.cvtColor(scaled_frame_aux1, cv2.COLOR_GRAY2BGR)

            if len(scaled_frame_aux2.shape) == 2:
                scaled_frame_aux2 = cv2.cvtColor(scaled_frame_aux2, cv2.COLOR_GRAY2BGR)

            left_screen = np.concatenate((self.frame_info, scaled_video_frame), axis=0)
            right_screen = np.concatenate((scaled_frame_aux1, scaled_frame_aux2), axis=0)
            screen = np.concatenate((left_screen, right_screen), axis=1)
            self.screen = screen
            self.dirty = False
        return self.screen

    def set_video_frame(self, frame):
        if frame is not None:
            self.frame_video = frame
            self.dirty = True

    def set_frame_aux1(self, frame):
        if frame is not None:
            self.frame_aux1 = frame
            self.dirty = True

    def set_frame_aux2(self, frame):
        if frame is not None:
            self.frame_aux2 = frame
            self.dirty = True

    def putText(self, text: str, line: int):
        self.info_lines[line] = text
        self.dirty = True
