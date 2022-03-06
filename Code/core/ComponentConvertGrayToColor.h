#ifndef HEADER_ComponentConvertGrayToColor
#define HEADER_ComponentConvertGrayToColor

#include <opencv2/opencv.hpp>
#include "Component.h"

//! A component that converts the input gray image to BGR.
class ComponentConvertGrayToColor: public Component {

public:
	//! Constructor.
	ComponentConvertGrayToColor(SwisTrackCore * stc);
	//! Destructor.
	~ComponentConvertGrayToColor();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();

	Component * Create() {
		return new ComponentConvertGrayToColor(mCore);
	}

private:
	Display mDisplayOutput;				//!< The DisplayImage showing the output of this component.

};

#endif

