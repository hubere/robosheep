import math
from collections import deque

import cv2

import Utils
from TrackedColorBlob import TrackedColorBlob
import numpy as np

from Utils import Point2f, Point
from globals import GUI

POSITION_HISTORY_BUFFER = 100


def distance(start: Point, end: Point):
    # return cv2.norm(start.tupel() - end.tupel())  <= gäht ned
    dx = end.x - start.x
    dy = end.y - start.y
    points_distance = math.sqrt(dx ** 2 + dy ** 2)
    return points_distance


class TrackedObject:

    def __init__(self):
        self.position = Point(0, 0)
        self.position_history = deque(maxlen=POSITION_HISTORY_BUFFER)
        self.direction = None

    def __str__(self):
        return "TrackedObject: pos: " + str(self.position) + " dir: " + str(self.direction)

    def getColorBlobs(self):
        color_blobs = []
        color_blobs.append(TrackedColorBlob())
        color_blobs.append(TrackedColorBlob())
        return color_blobs

    def set_position_and_direction(self, position: Point, direction=None):
        self.position = position
        self.position_history.appendleft(position)
        if direction is not None:
            self.direction = int(direction)
        else:
            # find point with certain distance
            last_point = position
            for x in self.position_history:
                if distance(x, position) > 10:
                    last_point = x
                    break
            if last_point is not None:
                self.direction = Utils.getKurswinkelDegree(last_point, position)

        GUI.putText(str(self), 10)
        pass

    def setDirection(self, direction: int):
        self.direction = direction

    def draw_position_history(self, frame):
        # loop over the set of tracked points
        for i in range(1, len(self.position_history)):
            # if either of the tracked points are None, ignore
            # them
            if self.position_history[i - 1] is None or self.position_history[i] is None:
                continue
            # otherwise, compute the thickness of the line and
            # draw the connecting lines
            thickness = int(np.sqrt(POSITION_HISTORY_BUFFER / float(i + 1)) * 2.5)
            cv2.line(frame, self.position_history[i - 1].tupel(), self.position_history[i].tupel(), (0, 0, 255),
                     thickness)
