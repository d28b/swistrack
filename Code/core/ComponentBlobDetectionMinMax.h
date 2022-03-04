#ifndef HEADER_ComponentBlobDetectionMinMax
#define HEADER_ComponentBlobDetectionMinMax

#include <opencv2/core.hpp>
#include "Component.h"
#include "BlobDetection.h"

//! A component that detects blobs that have a certain size and stores them as particles in DataStructureParticle.
class ComponentBlobDetectionMinMax: public Component {

public:
	//! Constructor.
	ComponentBlobDetectionMinMax(SwisTrackCore * stc);
	//! Destructor.
	~ComponentBlobDetectionMinMax();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();

	Component * Create() {
		return new ComponentBlobDetectionMinMax(mCore);
	}

private:
	BlobDetection mBlobDetection;
	Display mDisplayOutput;		//!< The Display showing the last acquired image and the particles.
};

#endif

