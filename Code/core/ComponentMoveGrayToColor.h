#ifndef HEADER_ComponentMoveGrayToColor
#define HEADER_ComponentMoveGrayToColor

#include <cv.h>
#include "Component.h"

//! A component that converts the input gray image to RGB.
class ComponentMoveGrayToColor: public Component {

public:
	//! Constructor.
	ComponentMoveGrayToColor(SwisTrackCore *stc);
	//! Destructor.
	~ComponentMoveGrayToColor();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {
		return new ComponentMoveGrayToColor(mCore);
	}

private:
	IplImage *mOutputImage;				//!< The image created by this component.
	Display mDisplayOutput;				//!< The DisplayImage showing the output of this component.

};

#endif

