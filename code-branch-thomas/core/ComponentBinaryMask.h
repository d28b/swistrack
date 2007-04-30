#ifndef HEADER_ComponentBinaryMask
#define HEADER_ComponentBinaryMask

#include <cv.h>
#include "Component.h"
#include "DisplayImageStandard.h"

//! A component that detects blobs that have a certain size and stores them as particles in DataStructureParticle.
class ComponentBinaryMask: public Component {

public:
	//! Constructor.
	ComponentBinaryMask(SwisTrackCore *stc);
	//! Destructor.
	~ComponentBinaryMask();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {return new ComponentBinaryMask(mCore);}

private:
	IplImage *mMaskImage;				//!< The mask.

	Display mDisplayOutput;				//!< The DisplayImage showing the output of this component.
};

#endif

