#ifndef HEADER_ComponentConvertBayerToColor
#define HEADER_ComponentConvertBayerToColor

#include <opencv2/core.hpp>
#include "Component.h"

//! A component that converts the input image (any type) to a grayscale image.
class ComponentConvertBayerToColor: public Component {

public:
	//! Constructor.
	ComponentConvertBayerToColor(SwisTrackCore * stc);
	//! Destructor.
	~ComponentConvertBayerToColor();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();

	Component * Create() {
		return new ComponentConvertBayerToColor(mCore);
	}

private:
	int mBayerType;						//!< (configuration) The Bayer Type of the camera
	Display mDisplayOutput;				//!< The DisplayImage showing the output of this component.
};

#endif

