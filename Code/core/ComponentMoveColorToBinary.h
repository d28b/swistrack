#ifndef HEADER_ComponentMoveColorToBinary
#define HEADER_ComponentMoveColorToBinary

#include <cv.h>
#include "Component.h"

//! A component that converts the color input image binary.
class ComponentMoveColorToBinary: public Component {

public:
	//! Constructor.
	ComponentMoveColorToBinary(SwisTrackCore *stc);
	//! Destructor.
	~ComponentMoveColorToBinary();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {
		return new ComponentMoveColorToBinary(mCore);
	}

private:
	IplImage *mOutputImage;				//!< The image created by this component.
	Display mDisplayOutput;				//!< The DisplayImage showing the output of this component.

};

#endif

