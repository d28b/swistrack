#ifndef HEADER_ComponentCamShiftTracking
#define HEADER_ComponentCamShiftTracking

#include <cv.h>
//#include <cvaux.hpp>
extern "C" {
#include "camshift_wrapper.h"
}
#include "Component.h"
#include "DataStructureParticles.h"

//! A basic tracking component that takes pre-filtered particales and
// adds them to the nearest track.
class ComponentCamShiftTracking: public Component {

public:
	//! Constructor.
	ComponentCamShiftTracking(SwisTrackCore *stc);
	//! Destructor.
	~ComponentCamShiftTracking();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {
		return new ComponentCamShiftTracking(mCore);
	}

private:
	void EraseTrack(int id);
	void UpdateTrackers(IplImage * inputImage);
	void AddNewTracks(IplImage * inputImage);
	void FilterTracks();
	void ClearTracks();
	int mNextTrackId;
	DataStructureParticles::tParticleVector mParticles;
	DataStructureTracks::tTrackMap mTracks;
	std::map<int, camshift> mTrackers;  // track id to tracker

	double mMinNewTrackDistanceSquared; //!< (configuration) The minimum distance between new tracks

	int mInitialWindowSize; //!< (configuration) The size of the initial window when you initialize a new tracker.

	int mHistogramDimensions; //!< (configuration)  Parameter for camshift
	int mVmin; //!< (configuration)  Parameter for camshift
	int mSmin; //!< (configuration)  Parameter for camshift
	unsigned int mMaximumNumberOfTrackers;

	IplImage * mOutputImage;
	int mFrameKillThreshold;
	int mTrackDistanceKillThresholdSquared;

	Display mDisplayOutput;									//!< The Display showing the last acquired image and the particles.
};

#endif

