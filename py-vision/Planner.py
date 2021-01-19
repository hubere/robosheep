from typing import Tuple

import Utils
from  Utils import Point
from TrackedObject import TrackedObject
from globals import GUI


class Planner:

    def __init__(self, trackedObject: TrackedObject):
        self.frame = None
        self.position = Point()
        self.aim = Point()
        self.proximity = 5
        self.trackedObject = trackedObject
        self.velocity = 0
        self.rotate = 0
        self.motorSpeed1 = 0
        self.motorSpeed2 = 0

    def set_aim(self, aim):
        self.aim = aim

    def plan(self):
        akt_pos: Point = self.trackedObject.position
        tiDegree, tsDegree = self.calc_direction_to_head(akt_pos)
        rotate = self.calc_movement_commands(tiDegree, tsDegree)
        self.motorSpeed1, self.motorSpeed2 = self.calc_motor_speeds(rotate)

        GUI.putText("Planer:", 20)
        GUI.putText(" aim=" + str(self.aim), 21)
        GUI.putText("Planer:", 22)
        GUI.putText("Planer:", 23)
        pass

    def calc_direction_to_head(self, pos: Point) -> Tuple[int, int]:
        tiDegree = Utils.getKurswinkelDegree(Point(), self.trackedObject.direction)  # Kurswinkel ist aus ImageAnalyser
        tsDegree = Utils.getKurswinkelDegree(pos, self.aim)  # Kurswinkel soll
        return tiDegree, tsDegree

    def calc_movement_commands(self, tsDegree: int, tiDegree: int ) -> int:
        rotate = tsDegree - tiDegree
        if rotate > 180:
            rotate = rotate - 360
        if rotate < -180:
            rotate = 360 + rotate
        return rotate

    def calc_motor_speeds(self, rotate: int) -> Tuple[int, int]:
        if rotate > 0:
            motorSpeed1 = self.velocity + self.velocity / 4;
            motorSpeed2 = self.velocity - self.velocity / 4;

        else:
            motorSpeed1 = self.velocity - self.velocity / 4;
            motorSpeed2 = self.velocity + self.velocity / 4;

        return motorSpeed1, motorSpeed2
