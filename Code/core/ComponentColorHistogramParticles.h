#ifndef HEADER_ComponentColorHistogramParticles
#define HEADER_ComponentColorHistogramParticles

#include <cv.h>
//#include <cvaux.hpp>
extern "C" {
#include "camshift_wrapper.h"
}
#include "Component.h"
#include "DataStructureParticles.h"

//! A basic tracking component that takes pre-filtered particales and
// adds them to the nearest track.
class ComponentColorHistogramParticles: public Component {

public:
	//! Constructor.
	ComponentColorHistogramParticles(SwisTrackCore *stc);
	//! Destructor.
	~ComponentColorHistogramParticles();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {
		return new ComponentColorHistogramParticles(mCore);
	}

private:
	CvRNG rng;
	
	IplImage * mInputChannels[3];

	IplImage * mParticleMask;
	IplImage * mOutputImage;

	int mMinArea;	//!< (configuration) The minimum area of a blob.


	
	Display mDisplayOutput;
};

#endif

