#ifndef HEADER_ComponentConvertInputToColor
#define HEADER_ComponentConvertInputToColor

#include <opencv2/core.hpp>
#include "Component.h"

//! A component that converts the input image (any format) to BGR.
class ComponentConvertInputToColor: public Component {

public:
	//! Constructor.
	ComponentConvertInputToColor(SwisTrackCore * stc);
	//! Destructor.
	~ComponentConvertInputToColor();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();

	Component * Create() {
		return new ComponentConvertInputToColor(mCore);
	}

private:
	Display mDisplayOutput;				//!< The DisplayImage showing the output of this component.

};

#endif

