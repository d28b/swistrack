#ifndef HEADER_ComponentSobelDifferentiation
#define HEADER_ComponentSobelDifferentiation

#include <cv.h>
#include "Component.h"
#include "DataStructureParticles.h"

//! A component tdifferentiates the image with Sobel().
class ComponentSobelDifferentiation: public Component {

public:
	//! Constructor.
	ComponentSobelDifferentiation(SwisTrackCore *stc);
	//! Destructor.
	~ComponentSobelDifferentiation();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	int xorder;
	int yorder;
	int apertureSize;
	int mChannelMain;					//!< The channel to subtract from.
	int mChannelSubtract;					//!< The channel to subtract.
	Display mDisplayOutput;					//!< The Display showing the last acquired image and the particles.
	Component *Create() {
		return new ComponentSobelDifferentiation(mCore);
	}
	
};

#endif

