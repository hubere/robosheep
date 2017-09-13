#include "Stopwatch.h"

using namespace std;
using namespace cv;

Stopwatch::Stopwatch()
{
	reset();
}


Stopwatch::~Stopwatch()
{
}

int Stopwatch::getElapsedTime()
{
	return (getTickCount() - frameProcessingStart)*1000. / getTickFrequency();
}

int Stopwatch::getNextFrameDelay(int frametime)
{
	int64 algtime = getElapsedTime();
	if (frametime > algtime)
		return frametime - algtime;
	else
		return 10;	// minimal framedelay}
}

void Stopwatch::reset()
{
	frameProcessingStart = getTickCount();
}




