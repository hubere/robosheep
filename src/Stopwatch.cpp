/*
* Implementation of Stopwatch.
*/

#include "Stopwatch.h"

using namespace std;
using namespace cv;

namespace robosheep {

Stopwatch::Stopwatch()
{
	reset();
}

Stopwatch::~Stopwatch()
{
}

int Stopwatch::getElapsedTime()
{
	return static_cast<int>((getTickCount() - frameProcessingStart)*1000. / getTickFrequency());
}

/*
* A convenience funtion. Returns a time difference between frametime an 
* elapsed time, floored by 10ms.
*/
int Stopwatch::getNextFrameDelay(int frametime)
{
	int64 algtime = getElapsedTime();
	if (frametime > algtime)
		return static_cast<int>(frametime - algtime);
	else
		return 10;	// minimal framedelay}
}

void Stopwatch::reset()
{
	frameProcessingStart = getTickCount();
}

}  // namespace robosheep


