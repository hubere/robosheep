import cv2
import numpy


class TrackedColorBlob:
    """

Datasets of color blobs.

 Use robosheep --analyse and type 't' to take a snapshot.
 Open the snapshot in gimp, choose color picker and doubleclick on selected color.
 That will open a color wheel where one can read HSV values.
    """

    def __init__(self, low, hi):
        self.tresh_low = low
        self.tresh_hi = hi


COLOR_BLOB_BLUE = TrackedColorBlob(numpy.array([92, 102, 134]), numpy.array([102, 219, 244]))
COLOR_BLOB_ORANGE = TrackedColorBlob(numpy.array([4, 145, 117]), numpy.array([15, 255, 255]))




