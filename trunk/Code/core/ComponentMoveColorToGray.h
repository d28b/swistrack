#ifndef HEADER_ComponentMoveColorToGray
#define HEADER_ComponentMoveColorToGray

#include <cv.h>
#include "Component.h"

//! A component that converts the color input image to gray.
class ComponentMoveColorToGray: public Component {

public:
	//! Constructor.
	ComponentMoveColorToGray(SwisTrackCore *stc);
	//! Destructor.
	~ComponentMoveColorToGray();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {
		return new ComponentMoveColorToGray(mCore);
	}

private:
	IplImage *mOutputImage;				//!< The image created by this component.
	Display mDisplayOutput;				//!< The DisplayImage showing the output of this component.

};

#endif

