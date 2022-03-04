#ifndef HEADER_ComponentConvertToGray
#define HEADER_ComponentConvertToGray

#include <opencv2/core.hpp>
#include "Component.h"

//! A component that converts the input image (any type) to a grayscale image.
class ComponentConvertToGray: public Component {

public:
	//! Constructor.
	ComponentConvertToGray(SwisTrackCore * stc);
	//! Destructor.
	~ComponentConvertToGray();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();

	Component * Create() {
		return new ComponentConvertToGray(mCore);
	}

private:
	int mChannel;						//!< (configuraiton) The channel number we use for the conversion.
	Display mDisplayOutput;				//!< The DisplayImage showing the output of this component.
};

#endif

