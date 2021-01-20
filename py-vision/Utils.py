from math import atan2, floor, pi
from typing import Tuple

import cv2
import numpy


class Point2f:
    def __init__(self, x=0.0, y=0.0):
        self.x = x
        self.y = y

    def tupel(self):
        return self.x, self.y

    def __str__(self):
        return "(" + str(self.x) + "/" + str(self.y) + ")"


class Point:
    def __init__(self, x=0, y=0):
        self.x = int(x)
        self.y = int(y)

    def tupel(self):
        return self.x, self.y

    def __str__(self):
        return "(" + str(self.x) + "/" + str(self.y) + ")"


class RRect:
    """
    taken from https://stackoverflow.com/questions/18207181/opencv-python-draw-minarearect-rotatedrect-not-implemented
    """
    def __init__(self, p0, s, ang):
        self.p0 = (int(p0[0]), int(p0[1]))
        (self.W, self.H) = s
        self.ang = ang
        self.p1, self.p2, self.p3 = self.get_verts(p0, s[0], s[1], ang)
        self.verts = [self.p0, self.p1, self.p2, self.p3]

    def get_verts(self, p0, W, H, ang):
        sin = numpy.sin(ang / 180 * 3.14159)
        cos = numpy.cos(ang / 180 * 3.14159)
        P1 = (int(self.H * sin) + p0[0], int(self.H * cos) + p0[1])
        P2 = (int(self.W * cos) + P1[0], int(-self.W * sin) + P1[1])
        P3 = (int(self.W * cos) + p0[0], int(-self.W * sin) + p0[1])
        return [P1, P2, P3]

    def draw(self, image):
        print(self.verts)
 #       for i in range(len(self.verts) - 1):
 #           cv2.line(image, (self.verts[i][0], self.verts[i][1]), (self.verts[i + 1][0], self.verts[i + 1][1]),
 #                    (0, 255, 0), 2)
#        cv2.line(image, (self.verts[3][0], self.verts[3][1]), (self.verts[0][0], self.verts[0][1]), (0, 255, 0), 2)

        contours = numpy.array([[self.verts[0][0], self.verts[0][1]], [self.verts[1][0], self.verts[1][1]], [self.verts[2][0], self.verts[2][1]],[self.verts[3][0], self.verts[3][1]]])
        cv2.fillPoly(image, pts=[contours], color=(0, 255, 0))


def getKurswinkel(Aufpunkt: Point, Zielpunkt: Point) -> float:
    """
    Berechung des Kurswinkels
    nach http://de.wikipedia.org/wiki/Richtungswinkel;
    ACHTUNG! dort wird die x-Achse nach Norden ausgerichtet und die Drehrichtung ist rechtsdrehend!)
    """
    return -atan2(Zielpunkt.y - Aufpunkt.y, Zielpunkt.x - Aufpunkt.x)


def getKurswinkelDegree(Aufpunkt: Point, Zielpunkt: Point) -> int:
    t = int(floor(getKurswinkel(Aufpunkt, Zielpunkt) / (pi / 180)))
    if t < 0:
        t += 360
    return t
