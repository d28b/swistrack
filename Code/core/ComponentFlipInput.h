#ifndef HEADER_ComponentFlipInput
#define HEADER_ComponentFlipInput

#include <opencv2/opencv.hpp>
#include "Component.h"

class ComponentFlipInput: public Component {

public:
	//! Constructor.
	ComponentFlipInput(SwisTrackCore * stc);
	//! Destructor.
	~ComponentFlipInput();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();

	Component * Create() {
		return new ComponentFlipInput(mCore);
	}

private:
	bool mFlipHorizontally;		//!< (configuration) Flip the image horizontally.
	bool mFlipVertically;		//!< (configuration) Flip the image vertically.
	Display mDisplayOutput;		//!< The DisplayImage showing the output of this component.

};

#endif

