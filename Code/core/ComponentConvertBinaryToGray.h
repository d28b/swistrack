#ifndef HEADER_ComponentConvertBinaryToGray
#define HEADER_ComponentConvertBinaryToGray

#include <opencv2/core.hpp>
#include "Component.h"

//! A component that converts the input binary image to BGR.
class ComponentConvertBinaryToGray: public Component {

public:
	//! Constructor.
	ComponentConvertBinaryToGray(SwisTrackCore * stc);
	//! Destructor.
	~ComponentConvertBinaryToGray();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();

	Component * Create() {
		return new ComponentConvertBinaryToGray(mCore);
	}

private:
	Display mDisplayOutput;				//!< The DisplayImage showing the output of this component.

};

#endif

