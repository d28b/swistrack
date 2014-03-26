#ifndef HEADER_ComponentBinaryErosion
#define HEADER_ComponentBinaryErosion

#include <cv.h>
#include "Component.h"

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
	Component *Create() {
		return new ComponentBinaryErosion(mCore);
	}

private:
	int mIterations;					//!< (configuration) The number of erosions to apply to the binary image.

	Display mDisplayOutput;				//!< The DisplayImage showing the output of this component.
};

#endif

