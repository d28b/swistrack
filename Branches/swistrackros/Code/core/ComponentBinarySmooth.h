#ifndef HEADER_ComponentBinarySmooth
#define HEADER_ComponentBinarySmooth

#include <cv.h>
#include "Component.h"
#include "DataStructureParticles.h"

//! A component smoothes binary images.
class ComponentBinarySmooth: public Component {

public:
	//! Constructor.
	ComponentBinarySmooth(SwisTrackCore *stc);
	//! Destructor.
	~ComponentBinarySmooth();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	int smoothtype;
	int param;
	int mChannelMain;					//!< The channel to subtract from.
	int mChannelSubtract;					//!< The channel to subtract.
	Display mDisplayOutput;					//!< The Display showing the last acquired image and the particles.
	Component *Create() {
		return new ComponentBinarySmooth(mCore);
	}
	
};

#endif

