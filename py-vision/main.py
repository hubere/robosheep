# This is a sample Python script.

# Press Shift+F10 to execute it or replace it with your code.
# Press Double Shift to search everywhere for classes, files, tool windows, actions, and settings.
import argparse
import os
import time

import cv2

from CountsPerSec import CountsPerSec
from VideoCamera import VideoCamera
from VideoShow import VideoShow
from globals import GUI
from gui import Gui
from thread_demo import putIterationsPerSec


def test_camera():
    url = args["cameraURL"]

    video_getter = VideoCamera(url).start()
    cps = CountsPerSec().start()

    print("Hit q to quit.")

    GUI.putText("Hit q to quit.", 25)

    while True:
        if (cv2.waitKey(1) == ord("q")) or video_getter.stopped:
            video_getter.stop()
            break

        frame = video_getter.frame
        frame = putIterationsPerSec(frame, cps.countsPerSec())
        cps.increment()
        GUI.set_video_frame(frame)
        screen = GUI.get_screen()
        cv2.imshow("Robosheep", screen)
        time.sleep(1)


def track():
    print("track not implemented yet.")
    pass


def analyse_image():
    print("analyse_image not implemented yet.")
    pass


def control_sheep():
    print("control_sheep not implemented yet.")
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
                    default="none",
                    help="URL of sheep controller.")
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
