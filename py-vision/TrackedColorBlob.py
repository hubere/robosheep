import cv2


class TrackedColorBlob:
    """

Datasets of color blobs.

 Use robosheep --analyse and type 't' to take a snapshot.
 Open the snapshot in gimp, choose color picker and doubleclick on selected color.
 That will open a color wheel where one can read HSV values.
    """

    def __init__(self, datasetidx: int):
        self.high_H = 70
        self.high_S = 30
        self.high_V = 97
        self.color_range = 20
        self.maxSize = None # TODO FIXME HU cv2.Size(10, 15);
        self.name = "GARDEN_YELLOW_DINA4"