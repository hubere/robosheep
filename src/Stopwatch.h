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

