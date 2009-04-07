#ifndef HEADER_Particle
#define HEADER_Particle
#include <wx/datetime.h>
#include <cv.h>

//! A particle.
class Particle {

public:
	int mID;					//!< ID of the particle, or of its associated track.
	float mIDCovariance;		//!< The covariance value of the ID detection algorithm.
	CvPoint2D32f mCenter;		//!< Particle center [pixels].
	float mOrientation;			//!< The orientation angle of the particle [rad].
	double mArea;				//!< Particle area [pixels^2].
	double mCompactness;		//!< The contour's compactness (area/circumference ratio).
	CvPoint2D32f mWorldCenter;	//!< Particle center after calibration [m].
	int mFrameNumber;

	CvHistogram * mColorModel;

	wxDateTime mTimestamp;

	//! Constructor.
	Particle(): mID(-1), mCenter(), mOrientation(0), mArea(0), mCompactness(0), mWorldCenter(), mColorModel(0), mTimestamp() {}
	//! Destructor.
	~Particle() {
	}

	//! Comparison function (comparison by area, largest first)
	static bool CompareArea(const Particle &p1, const Particle &p2) {
		return (p1.mArea > p2.mArea);
	}
};

#endif

