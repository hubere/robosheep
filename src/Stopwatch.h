/*
* A Stopwatch is used for time measurement. You can start it, and get elapsed
* time in ms since start.
* As a convenience funtion, getNextFrameDelay(int frametime) returns a time
* difference between frametime an elapsed time, floored by 10ms.
*
*  Created on: Nov 10, 2016
*      Author: Edwin Huber
*/

#pragma once

#include <opencv2/core/core.hpp>


class Stopwatch
{
private:
	int64 frameProcessingStart;

public:
	Stopwatch();
	~Stopwatch();
	int getElapsedTime();
	int getNextFrameDelay(int frametime);
	void reset();
};

