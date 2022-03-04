#ifndef HEADER_ComponentInvertBinary
#define HEADER_ComponentInvertBinary

#include <opencv2/core.hpp>
#include "Component.h"

//! A component that inverts a binary image.
class ComponentInvertBinary: public Component {

public:
	//! Constructor.
	ComponentInvertBinary(SwisTrackCore * stc);
	//! Destructor.
	~ComponentInvertBinary();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();

	Component * Create() {
		return new ComponentInvertBinary(mCore);
	}

private:
	Display mDisplayOutput;				//!< The DisplayImage showing the output of this component.
};

#endif

