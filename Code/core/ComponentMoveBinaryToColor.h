#ifndef HEADER_ComponentMoveBinaryToColor
#define HEADER_ComponentMoveBinaryToColor

#include <opencv2/core.hpp>
#include "Component.h"

//! A component that converts the input binary image to BGR.
class ComponentMoveBinaryToColor: public Component {

public:
	//! Constructor.
	ComponentMoveBinaryToColor(SwisTrackCore * stc);
	//! Destructor.
	~ComponentMoveBinaryToColor();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();

	Component * Create() {
		return new ComponentMoveBinaryToColor(mCore);
	}

private:
	Display mDisplayOutput;				//!< The DisplayImage showing the output of this component.

};

#endif

