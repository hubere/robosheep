import math
from math import pi, cos, sin

import cv2

import Utils
from Utils import Point2f, Point, RRect
from globals import GUI

COLOR_SHEEP = (255, 255, 0)


def rotate_image_without_cropping(mat, angle):
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


class VirtualSheep:

    def __init__(self, url: str):
        # parameter
        self.url = url
        self.sheep_image = cv2.imread("../resources/orange-blue.png")
        self.sheep_image = cv2.resize(self.sheep_image, (100, 100))
        self.sheep_image = rotate_image_without_cropping(self.sheep_image, 90)

        # desired motor speeds set via mowerController::move()
        # measured in real world motor speed (0-255)
        self.desired_speedM1 = 0
        self.desired_speedM2 = 0

        # calculated
        self.speedM1 = 0
        self.speedM2 = 0
        self.position = Point(500, 500)  # starting position
        self.dir = 0  # in degree
        self.velocity = 0  # in pixel speed

    def move(self, m1: int, m2: int):
        self.desired_speedM1 = m1
        self.desired_speedM2 = m2

    def update(self):
        self.adjust_motor_speeds_2_desired()
        self.adjust_velocity_and_dir()
        self.calculate_new_position()
        self.move_sheep_by_hand(GUI.last_key)

        GUI.putText("Sheep:", 16)
        GUI.putText(" pos  = " + str(self.position) + "    dir  = " + str(self.dir), 17)
        GUI.putText(" speed= (" + str(self.speedM1) + " / " + str(self.speedM2) + ")", 18)

    def adjust_motor_speeds_2_desired(self):
        MOTOR_SPEED_ADJUSTMENT = 1  # check if this does not toggle when chosen values > 1!
        if self.desired_speedM1 > self.speedM1:
            self.speedM1 += MOTOR_SPEED_ADJUSTMENT
        elif self.desired_speedM1 < self.speedM1:
            self.speedM1 -= MOTOR_SPEED_ADJUSTMENT
        if self.desired_speedM2 > self.speedM2:
            self.speedM2 += MOTOR_SPEED_ADJUSTMENT
        elif self.desired_speedM2 < self.speedM2:
            self.speedM2 -= MOTOR_SPEED_ADJUSTMENT
        pass

    def adjust_velocity_and_dir(self):
        pixel_speed_m1 = self.speedM1 / 1
        pixel_speed_m2 = self.speedM2 / 1
        self.velocity = int((pixel_speed_m1 + pixel_speed_m2) / 2)
        self.dir += pixel_speed_m1 - pixel_speed_m2
        if self.dir > 360:
            self.dir -= 360
        if self.dir < 0:
            self.dir += 360

    def calculate_new_position(self):
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
        pass

    def move_sheep_by_hand(self, key):
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
        if key == ord("y"):
            self.velocity = 0
            self.desired_speedM1 = 0
            self.desired_speedM2 = 0
        pass

    def draw_your_self(self, frame):
        if self.url != "simulateSheep":
            return

        if self.position != Point(0, 0):
            # draw image
            sheep_rotated = rotate_image_without_cropping(self.sheep_image, self.dir)
            frame = self.merge_image2_into_image1(frame, sheep_rotated)

            # draw position
            radius = 30
            cv2.circle(frame, self.position.tupel(), radius, COLOR_SHEEP, 4)

            # draw heading
            Utils.draw_direction(frame, self.position, self.dir, 80, COLOR_SHEEP)

    def merge_image2_into_image1(self, img1, img2):
        """
            # see https://opencv-python-tutroals.readthedocs.io/en/latest/py_tutorials/py_core/py_image_arithmetics/py_image_arithmetics.html
        :param frame:
        :param sheep_rotated:
        :return:
        """

        # I want to put logo on top-left corner, So I create a ROI
        rows, cols, channels = img2.shape
        x = self.position.x
        y = self.position.y
        ext_x = math.ceil(cols / 2)
        ext_y = math.ceil(rows / 2)
        roi = img1[y-math.floor(rows/2):y+math.ceil(rows/2), x-math.floor(cols/2):x+math.ceil(cols / 2)]

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
        img1[y-math.floor(rows/2):y+math.ceil(rows/2), x-math.floor(cols/2):x+math.ceil(cols / 2)] = dst
        return img1
