# This is a sample Python script.

# Press Shift+F10 to execute it or replace it with your code.
# Press Double Shift to search everywhere for classes, files, tool windows, actions, and settings.
import argparse
import os
import time
import cv2
from colorfilters import HSVFilter

from ImageAnalyser import ImageAnalyser
from MowerControler import MowerControler
from Planner import Planner
from TrackedObject import TrackedObject
from VideoCamera import VideoCamera
from VirtualSheep import VirtualSheep
from globals import GUI


def test_camera():
    video_getter = VideoCamera(args["cameraURL"]).start()

    GUI.putText("Hit q to quit.", 25)

    while not quit_loop():
        frame = video_getter.frame
        GUI.set_video_frame(frame)
        cv2.imshow("Robosheep", GUI.get_screen())
        time.sleep(0.1)  # show 10 fps


def analyse_image():
    """
    analyse image given by argument cameraURL
    """

    print("see https://github.com/alkasm/colorfilters")

    img = cv2.imread(args["cameraURL"])
    window = HSVFilter(img)
    window.show()

    print(f"Image filtered in HSV between {window.lowerb} and {window.upperb}.")
    pass


def track():
    video_getter = VideoCamera(args["cameraURL"]).start()
    trackedObject = TrackedObject()
    imageAnalyser = ImageAnalyser()

    GUI.putText("Hit q to quit.", 25)

    while True:

        frame = video_getter.frame
        imageAnalyser.detectObjectPositionByMoments(frame, trackedObject)
        trackedObject.draw_position_history(frame)

        GUI.set_video_frame(frame)
        GUI.update()

        if GUI.last_key == ord("q"):
            video_getter.stop()
            break

        time.sleep(0.1)  # show 10 fps
    pass


def quit_loop() -> bool:
    return GUI.last_key == ord("q")


def control_sheep():
    video_getter = VideoCamera(args["cameraURL"]).start()
    trackedObject = TrackedObject()
    imageAnalyser = ImageAnalyser(algorithm=args['algorithm'])
    planner = Planner(trackedObject)
    mowerControler = MowerControler(args["mowerURL"])
    virtualSheep = VirtualSheep(args["mowerURL"])

    GUI.putText("Middle click video to set aim.   '+/-' to increase/decrease speed.   Hit q to quit.", 25)

    while not quit_loop():
        frame = video_getter.frame
        virtualSheep.draw_your_self(frame)
        imageAnalyser.detect(frame, trackedObject)
        trackedObject.draw_position_history(frame)
        planner.plan()
        planner.draw_info(frame)
        virtualSheep.update()
        virtualSheep.move(planner.motorSpeed1, planner.motorSpeed2)
        mowerControler.move(planner.motorSpeed1, planner.motorSpeed2)

        GUI.set_video_frame(frame)
        GUI.update()
        time.sleep(0.1)  # show 10 fps

    video_getter.stop()
    pass


if __name__ == '__main__':

    # this is important for mibao cam.
    os.environ["OPENCV_FFMPEG_CAPTURE_OPTIONS"] = "rtsp_transport;udp"

    # parse arguments
    ap = argparse.ArgumentParser()
    ap.add_argument("--mode",
                    default="controlSheep",
                    help="Select feature(s) of robosheep",
                    choices=["testCamera", "track", "analyseImage", "controlSheep"])
    ap.add_argument("--cameraURL",
                    default="noCameraURL",
                    help="URL of video stream. As can be viewed by VLC")
    ap.add_argument("--mowerURL",
                    default="simulateSheep",
                    help="URL of sheep controller.")
    ap.add_argument("--algorithm",
                    default="momements",
                    help="algorithm used to find position and direction of sheep",
                    choices=["momements", "contours", "tracker", "backgroundSubstraction"])
    ap.add_argument("--thread", "-t",
                    default="none",
                    help="Threading mode: get (video read in its own thread),"
                         + " show (video show in its own thread), both"
                         + " (video read and video show in their own threads),"
                         + " none (default--no multithreading)")

    args = vars(ap.parse_args())

    if args["mode"] == "testCamera":
        test_camera()
    elif args["mode"] == "track":
        track()
    elif args["mode"] == "analyseImage":
        analyse_image()
    elif args["mode"] == "controlSheep":
        control_sheep()
    else:
        print("invalid argument --mode: " + args["mode"])
