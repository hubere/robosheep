from collections import deque

import cv2

from TrackedColorBlob import TrackedColorBlob
import numpy as np


POSITION_HISTORY_BUFFER = 100


class TrackedObject:

    def __init__(self):
        self.position = (0, 0)
        self.position_history = deque(maxlen=POSITION_HISTORY_BUFFER)

    def getColorBlobs(self):
        color_blobs = []
        color_blobs.append(TrackedColorBlob())
        color_blobs.append(TrackedColorBlob())
        return color_blobs

    def setAktualPos(self, position):
        self.position = position
        self.position_history.appendleft(position)
        pass

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
            cv2.line(frame, self.position_history[i - 1], self.position_history[i], (0, 0, 255), thickness)