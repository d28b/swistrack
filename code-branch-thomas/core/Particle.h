#ifndef HEADER_Particle
#define HEADER_Particle

#include "cv.h"

//! A particle.
class Particle {

public:
	int mID;				//!< ID of the particle, or of its associated track.
	CvPoint2D32f mCenter;	//!< Particle center.
	double mArea;			//!< Particle area.
	double mCompactness;	//!< The contour's compactness (area/circumference ratio).
	double mOrientation;	//!< The contour's orientation (not calculated at this time).

	//! Constructor.
	Particle(): mCenter(), mArea(0), mCompactness(0), mOrientation(0), mID(-1) {}
	//! Destructor.
	~Particle() {}
};

#endif

