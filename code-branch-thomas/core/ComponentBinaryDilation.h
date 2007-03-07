#ifndef HEADER_ComponentBinaryDilation
#define HEADER_ComponentBinaryDilation

#include "cv.h"
#include "Component.h"
#include "DataStructureParticles.h"
#include "DisplayImageParticles.h"

//! A component that detects blobs that have a certain size and stores them as particles in DataStructureParticle.
class ComponentBinaryDilation: public Component {

public:
	//! Constructor.
	ComponentBinaryDilation(SwisTrackCore *stc);
	//! Destructor.
	~ComponentBinaryDilation();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {return new ComponentBinaryDilation(mCore);}

private:
	int mNumber;	//!< (configuration) The number of dilations to apply to the binary image.

	DisplayImageStandard mDisplayImageOutput;				//!< The DisplayImage showing the last processed image.
};

#endif

