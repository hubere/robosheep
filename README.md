# Robosheep

## Overview
This is all code for robosheep application.
Robosheep will control a lawn mower by means of an IP camera. The camera will detect position of the lawn mower 
(robosheep) and commands will be sent over WiFi to control mowers movements.

## Requirements
* Python 3.6 or above, with packages as listed in requirements.txt

## Components / folder structure
### docu (documentation)
Contains all documentation about this project. There is also a wordpress page at http://boezerbastler.wordpress.com
 
### gui (sheep controller UI)
A flask server that serves gui/plain/templates/index.html. This Java Script web site will be deployed onto the robosheep
controller. It gives status information and has widgets to move the robosheep (short: sheep) 
<p> <b>gui/plain/templates/index.html</b> has to stay in sync with <b>sketchbook/Motordriver/test.html</b> which in turn
needs to be kept in sync with <b>sketchbook/Motordriver/HTML.h</b> (manual modification of HTML.h is required!).
<p>Use run configuration: <b>Flask (gui/plain/app.py)</b>

### py-vision (eye of the sheep)
This python module handles the vision and command calculation code. There are a couple of modes this module can be 
driven:
<p><b>test Camera</b> to test camera operations</p>
<p><b>analyse Image</b> to produce upper and lower bound of color blobs</p>
<p><b>track Object</b> see tracking algorithms in action</p>
<p><b>control simulated sheep on image</b> to do a full closed loop processing with a simulated sheep on an garden image</p>
<p><b>control sheep</b> is the main functionality. Take image, find sheep position and direction, plan aim, calculate
control commands, send commands to sheep.</p>

1) fetch image (-> VideoCamera.py)
2) find sheep position and direction (-> ImageAnalyser.py)
3) plan aim and calculate commands (-> Planner.py)
4) send commands to sheep (-> MowerController.py)


### resources
Basically images and videos of garden. Used to do some analysis.

### sketchbook (sheep controller)
This is an Arduino script that is flashed onto sheep itself. It contains a web server that serves a JavaScript page. 
(see <b>gui (sheep controller UI)</b>)  


## Usage


