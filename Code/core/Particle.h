#ifndef HEADER_Particle
#define HEADER_Particle
#include <wx/datetime.h>
#include <opencv2/core.hpp>

//! A particle.
class Particle {

public:
	int mID;					//!< ID of the particle, or of its associated track.
	float mIDCovariance;		//!< The covariance value of the ID detection algorithm.
	cv::Point2f mCenter;		//!< Particle center [pixels].
	float mOrientation;			//!< The orientation angle of the particle [rad].
	double mArea;				//!< Particle area [pixels^2].
	double mCompactness;		//!< The contour's compactness (area/circumference ratio).
	cv::Point2f mWorldCenter;	//!< Particle center after calibration [m].

	cv::Mat mColorModel;		//!< Color histogram of the particle.

	int mFrameNumber;			//!< Frame number.
	wxDateTime mFrameTimestamp;	//!< Frame timestamp.

	//! Constructor.
	Particle(): mID(-1), mCenter(), mOrientation(0), mArea(0), mCompactness(0), mWorldCenter(), mColorModel(), mFrameNumber(0), mFrameTimestamp() { }
	//! Destructor.
	~Particle() { }

	//! Comparison function (comparison by area, largest first)
	static bool CompareArea(const Particle & p1, const Particle & p2) {
		return p1.mArea > p2.mArea;
	}
};

#endif
