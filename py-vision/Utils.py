from math import atan2, floor, pi
from typing import Tuple


class Point2f:
    def __init__(self, x=0.0, y=0.0):
        self.x = x
        self.y = y

    def tupel(self):
        return self.x, self.y

    def __str__(self):
        return "("+str(self.x)+"/"+str(self.y)+")"


class Point:
    def __init__(self, x=0, y=0):
        self.x = x
        self.y = y

    def tupel(self):
        return self.x, self.y

    def __str__(self):
        return "(" + str(self.x) + "/" + str(self.y) + ")"


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
