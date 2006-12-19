#ifndef HEADER_Particle
#define HEADER_Particle

#include "cv.h"

/** \class particle
* \brief Stores parameters and track id of a particle.
* A particle allows for storing its center (sub-pixel accuracy), its size, compactness, orientation, color, and the id 
* of its associated track.
*/
class Particle {

public:
	//! Associated track
	int mID;
	/** Particle center */
	CvPoint2D32f mCenter;
	/** Particle area */
	double mArea;
	/** The contour's compactness (area/circumference ratio) */
	double mCompactness;
	/** The contour's orientation (not calculated at this time) */
	double mOrientation;

	//! Constructor.
	Particle(): mCenter(0, 0), mArea(0), mCompactness(0), mOrientation(0), mID(-1) {}
	~Particle() {}
};

#endif

