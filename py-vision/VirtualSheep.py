from math import pi, cos, sin

import cv2

from Utils import Point2f, Point
from globals import GUI


class VirtualSheep:

    def __init__(self, url: str):
        self.url = url
        self.position = Point(50, 50)
        self.lastpos = Point(0, 0)
        self.speedM1 = 0
        self.speedM2 = 0
        self.dir = 45
        self.velocity = 1
        self.size = (10, 15)
        self.yellow = (0, 255, 255)  # yellow
        self.blue = (55, 177, 218)
        self.orange = (230, 117, 44)

    def draw_your_self(self, frame):
        if self.url != "simulateSheep":
            return

        #
        # draw sheep in original frame
        #
        if self.position != Point(0, 0):
            start_point = (self.position.x - self.size[0], self.position.y - self.size[1])
            end_point = (self.position.x + self.size[0], self.position.y + self.size[1])
            thickness = 8
            image = cv2.rectangle(frame, start_point, end_point, self.blue, thickness)

        if self.lastpos != Point(0, 0):
            cv2.line(frame, self.lastpos.tupel(), self.position.tupel(), (255, 0, 0), 2)
            cv2.circle(frame, self.position.tupel(), 4, (255, 0, 0), 2)

        #
        # ouput text in copy of frame
        #

        # sprintf(buffer, "Use 'w','y','a','s','+','-' to control sheep");
        # putText(newFame, buffer, Point(40, 50), FONT_HERSHEY_COMPLEX_SMALL, 1, Scalar::all(255), 1, 8);
        # sprintf(buffer, "(%.0f,%.0f): v=%d, d=%d\n", pos.x, pos.y, velocity, dir);
        # putText(newFame, buffer, Point(40, 100), FONT_HERSHEY_COMPLEX_SMALL, 1, Scalar::all(255), 1, 8);

    def move(self, motorSpeed1: int, motorSpeed2: int):
        self.speedM1 = motorSpeed1
        self.speedM2 = motorSpeed2

    def update(self):
        if self.position == Point():
            return

        radian = self.dir * (pi / 180)
        incX = cos(radian)
        incY = -sin(radian)

        if self.dir > 90:
            incX = cos(pi / 180 - radian)
            incY = sin(pi / 180 - radian)

        if self.dir > 180:
            incX = cos(radian)
            incY = -sin(radian)

        if self.dir > 270:
            incX = cos(radian)
            incY = -sin(radian)

        self.position = Point(int(self.position.x + incX * self.velocity), int(self.position.y + incY * self.velocity))

        if self.position.x < 0:
            self.position.x = 0
        if self.position.y < 0:
            self.position.y = 0
        if self.position.x > 500:  # TODO FIXME HU 500 seems max width / hight
            self.position.x = 500
        if self.position.y > 500:
            self.position.y = 500

        GUI.putText("Sheep:", 16)
        GUI.putText(" pos  = " + str(self.position) + "    dir  = " + str(self.dir), 17)
        GUI.putText(" speed= (" + str(self.speedM1) + " / " + str(self.speedM2) + ")", 18)