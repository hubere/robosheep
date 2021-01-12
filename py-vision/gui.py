from threading import Thread
from time import sleep

import cv2
import numpy as np


# orig 1920 x 1080
HIGHT = 540
WIDTH = 960


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

    def get_screen(self):
        if self.dirty:
            self.frame_info = np.zeros((HIGHT, WIDTH, 3), np.uint8)
            for num, info in enumerate(self.info_lines, start=1):
                cv2.putText(self.frame_info, info, (10, num * 20), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 255))

            scaled_video_frame = cv2.resize(self.frame_video, (WIDTH, HIGHT))
            scaled_frame_aux1 = cv2.resize(self.frame_aux1, (WIDTH, HIGHT))
            scaled_frame_aux2 = cv2.resize(self.frame_aux2, (WIDTH, HIGHT))

            scaled_frame_aux2 = cv2.cvtColor(scaled_frame_aux2, cv2.COLOR_GRAY2BGR)

            left_screen = np.concatenate((self.frame_info, scaled_video_frame), axis=0)
            right_screen = np.concatenate((scaled_frame_aux1, scaled_frame_aux2), axis=0)
            screen = np.concatenate((left_screen, right_screen), axis=1)
            self.screen = screen
            self.dirty = False
        return self.screen

    def set_video_frame(self, frame):
        self.frame_video = frame
        self.dirty = True

    def set_frame_aux1(self, frame):
        self.frame_aux1 = frame
        self.dirty = True

    def set_frame_aux2(self, frame):
        self.frame_aux2 = frame
        self.dirty = True

    def putText(self, text: str, line: int):
        self.info_lines[line] = text
        self.dirty = True
