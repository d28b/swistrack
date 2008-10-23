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
	int mNextTrackId;
	camshift cs;
	DataStructureParticles::tParticleVector mParticles;
	DataStructureTracks::tTrackMap mTracks;
	std::map<int, camshift> mTrackers;  // track id to tracker
	
	

	double** distanceArray;
	int maxParticles;

	//CvCamShiftTracker mTracker;

	IplImage * mOutputImage;

	// Parameters
	unsigned int mWindowSize; //!< (configuration) The size of the window to cache
	int mMaxNumber;		//!< (configuration) The maximum number of objects that are to track.
	Display mDisplayOutput;									//!< The Display showing the last acquired image and the particles.
};

#endif

