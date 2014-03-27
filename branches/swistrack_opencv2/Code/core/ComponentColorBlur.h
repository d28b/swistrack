#ifndef HEADER_ComponentColorBlur
#define HEADER_ComponentColorBlur

#include <cv.h>
#include "Component.h"

//! A component that detects blobs that have a certain size and stores them as particles in DataStructureParticle.
class ComponentColorBlur: public Component {

public:
	//! Constructor.
	ComponentColorBlur(SwisTrackCore *stc);
	//! Destructor.
	~ComponentColorBlur();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {
		return new ComponentColorBlur(mCore);
	}

private:
	int mBlurType;						//!< (configuration) The type of blur to apply.
	int mRadius;						//!< (configuration) The radius.

	Display mDisplayOutput;				//!< The DisplayImage showing the output of this component.
};

#endif

