import math
from math import cos, sin
from typing import Tuple

import cv2

import Utils
from  Utils import Point
from TrackedObject import TrackedObject
from globals import GUI


COLOR_PLANNER = (0, 255, 0)

class Planner:

    def __init__(self, trackedObject: TrackedObject):
        self.aim = None
        self.proximity = 5
        self.trackedObject = trackedObject
        self.velocity = 0
        self.direction = 0
        self.motorSpeed1 = 0
        self.motorSpeed2 = 0
        # used for initialization
        self.init_start = None
        self.init_wend = None
        self.init_end = None
        self.angle_in_10_iterations = None
        self.init_moves = 0


    def set_aim(self, aim):
        self.aim = aim

    def plan(self):
        if GUI.last_click != self.aim:
            self.aim = GUI.last_click

        if GUI.last_key == ord("+"):
            self.velocity += 10

        if GUI.last_key == ord("-"):
            self.velocity -= 10

        if not self.trackedObject.initialized:
            MOTOR_SPEED = 80
            self.init_moves += 1
            if self.init_moves == 1:  # run in straight forward for 10 iterations
                self.init_start = self.trackedObject.position
                self.motorSpeed1 = MOTOR_SPEED
                self.motorSpeed2 = MOTOR_SPEED

            elif self.init_moves == 10:  # slow down
                self.motorSpeed1 = 0
                self.motorSpeed2 = 0

            elif self.init_moves == 20:  # rotate for 4 iterations
                self.direction = Utils.getKurswinkelDegree(self.init_start, self.trackedObject.position)
                self.motorSpeed1 = MOTOR_SPEED
                self.motorSpeed2 = -MOTOR_SPEED

            elif self.init_moves == 28:  # come to a still
                self.motorSpeed1 = 0
                self.motorSpeed2 = 0

            elif self.init_moves == 40:  # run straight in another direction
                self.init_wend = self.trackedObject.position
                self.motorSpeed1 = MOTOR_SPEED
                self.motorSpeed2 = MOTOR_SPEED

            elif self.init_moves == 60:  # come to a still
                # init moves done
                self.init_end = self.trackedObject.position
                self.motorSpeed1 = 0
                self.motorSpeed2 = 0
                new_direction = Utils.getKurswinkelDegree(self.init_wend, self.init_end)
                self.angle_in_10_iterations = self.direction - new_direction
                self.direction = new_direction

                print("edi")



        if self.aim is not None:
            akt_pos: Point = self.trackedObject.position
            ti_degree = self.trackedObject.direction
            ts_degree = self.calc_direction_to_head(akt_pos)
            if ti_degree is not None:
                rotate = self.calc_movement_commands(ti_degree, ts_degree)
                self.motorSpeed1, self.motorSpeed2 = self.calc_motor_speeds(rotate)
            else:
                # start moving in any direction
                self.motorSpeed1, self.motorSpeed2 = self.calc_motor_speeds(None)

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
        if rotate is None:
            motor_speed1 = + self.velocity / 4
            motor_speed2 = + self.velocity / 4

        elif rotate > 90:
            motor_speed1 = - self.velocity / 4
            motor_speed2 = + self.velocity / 4

        elif rotate > 10:
            motor_speed1 = 0
            motor_speed2 = + self.velocity / 4

        elif rotate > -10:
            motor_speed1 = + self.velocity / 4
            motor_speed2 = + self.velocity / 4

        elif rotate > -90:
            motor_speed1 = + self.velocity / 4
            motor_speed2 = 0

        else:
            motor_speed1 = + self.velocity / 4
            motor_speed2 = - self.velocity / 4

#        if rotate > 0:
#            motor_speed1 = self.motorSpeed1 - self.velocity / 4
#            motor_speed2 = self.motorSpeed2 + self.velocity / 4

#        else:
#            motor_speed1 = self.motorSpeed1 + self.velocity / 4
#            motor_speed2 = self.motorSpeed2 - self.velocity / 4

        return int(motor_speed1), int(motor_speed2)

    def draw_info(self, frame):
        radius = 40
        if self.aim is not None:
            # draw aim
            cv2.circle(frame, self.aim.tupel(), radius, COLOR_PLANNER, 4)

            # draw heading
            ts_degree = self.calc_direction_to_head(self.trackedObject.position)
            Utils.draw_direction(frame, self.trackedObject.position, ts_degree, 80, COLOR_PLANNER )

            GUI.putText("Planer:", 20)
            GUI.putText(" aim=" + str(self.aim) + " vel=" + str(self.velocity), 21)
            GUI.putText(" heading: " + str(self.trackedObject.direction), 22)

        pass

