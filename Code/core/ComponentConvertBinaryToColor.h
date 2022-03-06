#ifndef HEADER_ComponentConvertBinaryToColor
#define HEADER_ComponentConvertBinaryToColor

#include <opencv2/core.hpp>
#include "Component.h"

//! A component that converts the input binary image to BGR.
class ComponentConvertBinaryToColor: public Component {

public:
	//! Constructor.
	ComponentConvertBinaryToColor(SwisTrackCore * stc);
	//! Destructor.
	~ComponentConvertBinaryToColor();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();

	Component * Create() {
		return new ComponentConvertBinaryToColor(mCore);
	}

private:
	Display mDisplayOutput;				//!< The DisplayImage showing the output of this component.

};

#endif

