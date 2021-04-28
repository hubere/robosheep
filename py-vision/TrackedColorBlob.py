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



#COLOR_BLOB_BLUE_1 = TrackedColorBlob(numpy.array([ 89,  62, 181]), numpy.array([ 98, 172, 242])) # 2021y3M8d8h54m52s.jpg
#COLOR_BLOB_RED__1 = TrackedColorBlob(numpy.array([  0,  73, 216]), numpy.array([ 12, 113, 240])) # 2021y3M8d8h54m52s.jpg

#COLOR_BLOB_BLUE_2 = TrackedColorBlob(numpy.array([ 90,  68, 195]), numpy.array([101, 153, 239])) # 2021y3M8d8h55m6s.jpg
#COLOR_BLOB_RED__2 = TrackedColorBlob(numpy.array([  0,  81, 196]), numpy.array([ 10, 111, 249])) # 2021y3M8d8h55m6s.jpg

#COLOR_BLOB_BLUE_3 = TrackedColorBlob(numpy.array([ 90,  68, 195]), numpy.array([101, 153, 239])) # 2021y3M8d8h55m21s.jpg
#COLOR_BLOB_RED__3 = TrackedColorBlob(numpy.array([  0,  81, 196]), numpy.array([ 10, 111, 249])) # 2021y3M8d8h55m21s.jpg

# vlcsnap-2021-03-23-16h41m32s576.png
COLOR_BLOB_BLUE_1  = TrackedColorBlob(numpy.array([ 87,  57, 112]), numpy.array([104, 247, 255]))
COLOR_BLOB_WHITE_1 = TrackedColorBlob(numpy.array([  0,   0, 255]), numpy.array([  0,   0, 255]))

# vlcsnap-2021-03-24-10h30m30s535.png
COLOR_BLOB_BLUE_2  = TrackedColorBlob(numpy.array([ 79,  17, 126]), numpy.array([123, 221, 255]))
COLOR_BLOB_WHITE_2 = TrackedColorBlob(numpy.array([0, 0, 0]), numpy.array([  0,   0, 255]))
