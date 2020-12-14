# robosheep

This is the movement planning part of robosheep.

The source is published at https://github.com/hubere/robosheep
A Blog is here https://boezerbastler.wordpress.com/


Usage:

Preperatory steps:
1) Switch on robosheep and camera
2) find IP addresses of robosheep (Espressif) and camera (DCS-930LB) with fing-app.
3) Check if robosheep controller is running at http:://<robosheepIP>:5000
4) Check if camera is working at http://<cameraIP> (admin/hubercek)



Start with no arguments

If camera does not show up, provide URL, e.g. --cameraURL http://192.168.1.105/video.cgi?x.mjpg

If no contour can be found, adjust colorBlobs.

	Adjust ColorBlobs:
	
		1. Run robosheep --analyse
		2. Hit 't' to take a snapshot with mover in each corner of garden
		3. Open the snapshot in gimp
		4. Choose color picker tool, pick a color and doubleclick on selected color. That will open a color wheel.
		5. Read HSV values and put them in TrackedColorBlob.cpp



