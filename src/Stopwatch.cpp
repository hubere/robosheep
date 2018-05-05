/*
* Implementation of Stopwatch.
*/

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

int64 Stopwatch::getElapsedTime()
{
	return static_cast<int64>((getTickCount() - frameProcessingStart)*1000. / getTickFrequency());
}

/*
* A convenience funtion. Returns a time difference between frametime an 
* elapsed time, floored by 10ms.
*/
int64 Stopwatch::getNextFrameDelay(int frametime)
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




