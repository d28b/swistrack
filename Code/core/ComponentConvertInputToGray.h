#ifndef HEADER_ComponentConvertInputToGray
#define HEADER_ComponentConvertInputToGray

#include <opencv2/core.hpp>
#include "Component.h"

//! A component that converts the input image (any type) to a grayscale image.
class ComponentConvertInputToGray: public Component {

public:
	//! Constructor.
	ComponentConvertInputToGray(SwisTrackCore * stc);
	//! Destructor.
	~ComponentConvertInputToGray();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();

	Component * Create() {
		return new ComponentConvertInputToGray(mCore);
	}

private:
	int mChannel;						//!< (configuraiton) The channel number we use for the conversion.
	Display mDisplayOutput;				//!< The DisplayImage showing the output of this component.
};

#endif

