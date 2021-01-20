import math
from math import pi, cos, sin

import cv2

from Utils import Point2f, Point, RRect
from globals import GUI


class VirtualSheep:

    def __init__(self, url: str):
        self.url = url
        self.sheep_image = cv2.imread("../resources/orange-blue.png")
        self.position = Point(500, 500)
        self.lastpos = Point(500, 500)
        self.speedM1 = 0
        self.speedM2 = 0
        self.dir = 45
        self.velocity = 0
        self.sheep_image = cv2.resize(self.sheep_image, (100, 100))

    def draw_your_self(self, frame):
        if self.url != "simulateSheep":
            return

        if self.position != Point(0, 0):
            sheep_rotated = self.rotate_image_without_cropping(self.sheep_image, self.dir)
            frame = self.merge_image2_into_image1(frame, sheep_rotated)


    def merge_image2_into_image1(self, img1, img2):
        """
            # see https://opencv-python-tutroals.readthedocs.io/en/latest/py_tutorials/py_core/py_image_arithmetics/py_image_arithmetics.html
        :param frame:
        :param sheep_rotated:
        :return:
        """

        # I want to put logo on top-left corner, So I create a ROI
        rows, cols, channels = img2.shape
        roi = img1[self.position.y:self.position.y + rows, self.position.x:self.position.x + cols]

        # Now create a mask of logo and create its inverse mask also
        img2gray = cv2.cvtColor(img2, cv2.COLOR_BGR2GRAY)
        ret, mask = cv2.threshold(img2gray, 10, 255, cv2.THRESH_BINARY)
        mask_inv = cv2.bitwise_not(mask)

        # Now black-out the area of logo in ROI
        img1_bg = cv2.bitwise_and(roi, roi, mask=mask_inv)

        # Take only region of logo from logo image.
        img2_fg = cv2.bitwise_and(img2, img2, mask=mask)

        # Put logo in ROI and modify the main image
        dst = cv2.add(img1_bg, img2_fg)
        img1[self.position.y:self.position.y + rows, self.position.x:self.position.x + cols] = dst
        return img1

    def rotate_image_without_cropping(self, mat, angle):
        """
        see https://stackoverflow.com/questions/22041699/rotate-an-image-without-cropping-in-opencv-in-c/33564950#33564950
        :param mat:
        :param angle:
        :return:
        """
        height, width = mat.shape[:2]
        image_center = (width / 2, height / 2)

        rotation_mat = cv2.getRotationMatrix2D(image_center, angle, 1)

        radians = math.radians(angle)
        sin = math.sin(radians)
        cos = math.cos(radians)
        bound_w = int((height * abs(sin)) + (width * abs(cos)))
        bound_h = int((height * abs(cos)) + (width * abs(sin)))

        rotation_mat[0, 2] += ((bound_w / 2) - image_center[0])
        rotation_mat[1, 2] += ((bound_h / 2) - image_center[1])

        rotated_mat = cv2.warpAffine(mat, rotation_mat, (bound_w, bound_h))
        return rotated_mat

    def move(self, m1: int, m2: int):
        self.speedM1 = m1
        self.speedM2 = m2

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
        if self.position.x > 1920:  # TODO FIXME HU 500 seems max width / hight
            self.position.x = 1920
        if self.position.y > 1080:
            self.position.y = 1080

        key = cv2.waitKey(100)
        if key == ord("a"):
            self.position.x -= 10
        if key == ord("d"):
            self.position.x += 10
        if key == ord("w"):
            self.position.y -= 10
        if key == ord("s"):
            self.position.y += 10
        if key == ord("y"):
            self.dir += 10
        if key == ord("c"):
            self.dir -= 10

        GUI.putText("Sheep:", 16)
        GUI.putText(" pos  = " + str(self.position) + "    dir  = " + str(self.dir), 17)
        GUI.putText(" speed= (" + str(self.speedM1) + " / " + str(self.speedM2) + ")", 18)
