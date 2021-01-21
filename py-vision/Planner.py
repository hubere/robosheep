from typing import Tuple

import cv2

import Utils
from  Utils import Point
from TrackedObject import TrackedObject
from globals import GUI


class Planner:

    def __init__(self, trackedObject: TrackedObject):
        self.aim = None
        self.proximity = 5
        self.trackedObject = trackedObject
        self.velocity = 0
        self.motorSpeed1 = 0
        self.motorSpeed2 = 0

    def set_aim(self, aim):
        self.aim = aim

    def plan(self):
        if GUI.last_click != self.aim:
            self.aim = GUI.last_click

        if GUI.last_key == ord("+"):
            self.velocity += 10

        if GUI.last_key == ord("-"):
            self.velocity -= 10

        if self.aim is not None:
            akt_pos: Point = self.trackedObject.position
            ti_degree = self.trackedObject.direction
            ts_degree = self.calc_direction_to_head(akt_pos)
            if ti_degree is not None:
                rotate = self.calc_movement_commands(ti_degree, ts_degree)
                self.motorSpeed1, self.motorSpeed2 = self.calc_motor_speeds(rotate)

        GUI.putText("Planer:", 20)
        GUI.putText(" aim=" + str(self.aim) + " vel=" + str(self.velocity), 21)
        GUI.putText(" :", 22)
        pass

    def calc_direction_to_head(self, pos: Point) -> int:
        ts_degree = Utils.getKurswinkelDegree(pos, self.aim)  # Kurswinkel soll
        return ts_degree

    def calc_movement_commands(self, ti_degree: int, ts_degree: int) -> int:
        rotate = ts_degree - ti_degree
        if rotate > 180:
            rotate = rotate - 360
        if rotate < -180:
            rotate = 360 + rotate
        return rotate

    def calc_motor_speeds(self, rotate: int) -> Tuple[int, int]:
        if rotate > 0:
            motor_speed1 = self.velocity + self.velocity / 4
            motor_speed2 = self.velocity - self.velocity / 4

        else:
            motor_speed1 = self.velocity - self.velocity / 4
            motor_speed2 = self.velocity + self.velocity / 4

        return int(motor_speed1), int(motor_speed2)

    def draw_info(self, frame):
        radius = 40
        if self.aim is not None:
            cv2.circle(frame, self.aim.tupel(), radius, (0, 255, 0), 4)
        pass

