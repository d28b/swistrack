#ifndef HEADER_ComponentConvertColorToGray
#define HEADER_ComponentConvertColorToGray

#include <opencv2/opencv.hpp>
#include "Component.h"

//! A component that converts the color input image to gray.
class ComponentConvertColorToGray: public Component {

public:
	//! Constructor.
	ComponentConvertColorToGray(SwisTrackCore * stc);
	//! Destructor.
	~ComponentConvertColorToGray();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();

	Component * Create() {
		return new ComponentConvertColorToGray(mCore);
	}

private:
	int mChannel;						//!< (configuraiton) The channel number we use for the conversion.
	Display mDisplayOutput;				//!< The DisplayImage showing the output of this component.

};

#endif

