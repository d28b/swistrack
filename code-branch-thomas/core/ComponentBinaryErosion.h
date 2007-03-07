#ifndef HEADER_ComponentBinaryErosion
#define HEADER_ComponentBinaryErosion

#include "cv.h"
#include "Component.h"
#include "DataStructureParticles.h"
#include "DisplayImageParticles.h"

//! A component that detects blobs that have a certain size and stores them as particles in DataStructureParticle.
class ComponentBinaryErosion: public Component {

public:
	//! Constructor.
	ComponentBinaryErosion(SwisTrackCore *stc);
	//! Destructor.
	~ComponentBinaryErosion();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {return new ComponentBinaryErosion(mCore);}

private:
	int mNumber;	//!< (configuration) The number of erosions to apply to the binary image.

	DisplayImageStandard mDisplayImageOutput;				//!< The DisplayImage showing the last processed image.
};

#endif

