#ifndef HEADER_Track
#define HEADER_Track

#include <deque>
#include <cmath>
#include <opencv2/core.hpp>

class TrackPoint {
public:
	float x;
	float y;
	int frameNumber;

	TrackPoint(): x(0.0f), y(0.0f), frameNumber(0) { }
	TrackPoint(float x, float y, int frameNumber): x(x), y(y), frameNumber(frameNumber) { }
};

class Track {
public:
	int mID;								//!< ID.
	std::deque<TrackPoint> mTrajectory;		//!< Trajectory.

	Track();
	~Track();

	void AddPoint(float x, float y, int frameNumber);
	void Shorten(unsigned int maxNumberOfPoints);
};

#endif
