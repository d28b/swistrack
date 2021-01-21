#ifndef HEADER_ComponentBinaryDilation
#define HEADER_ComponentBinaryDilation

#include <cv.h>
#include "Component.h"

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
	Component *Create() {
		return new ComponentBinaryDilation(mCore);
	}

private:
	int mIterations;					//!< (configuration) The number of dilations to apply to the binary image.

	Display mDisplayOutput;				//!< The DisplayImage showing the output of this component.
};

#endif

