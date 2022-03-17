#include "Track.h"
#define THISCLASS Track

THISCLASS::Track(): mID(-1), mTrajectory() {
}

THISCLASS::~Track() {
}

void THISCLASS::AddPoint(float x, float y, int frameNumber) {
	mTrajectory.push_back(TrackPoint(x, y, frameNumber));
}

void THISCLASS::Shorten(unsigned int maxNumberOfPoints) {
	while (mTrajectory.size() > maxNumberOfPoints)
		mTrajectory.pop_front();
}
