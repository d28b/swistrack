#ifndef HEADER_ComponentConvertToColor
#define HEADER_ComponentConvertToColor

#include <opencv2/core.hpp>
#include "Component.h"

//! A component that converts the input image (any format) to BGR.
class ComponentConvertToColor: public Component {

public:
	//! Constructor.
	ComponentConvertToColor(SwisTrackCore * stc);
	//! Destructor.
	~ComponentConvertToColor();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();

	Component * Create() {
		return new ComponentConvertToColor(mCore);
	}

private:
	Display mDisplayOutput;				//!< The DisplayImage showing the output of this component.
};

#endif

