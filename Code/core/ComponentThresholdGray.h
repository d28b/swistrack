#ifndef HEADER_ComponentThresholdGray
#define HEADER_ComponentThresholdGray

#include <opencv2/opencv.hpp>
#include "Component.h"

//! A component that applies a constant threshold to a grayscale image. The output is a binary image.
class ComponentThresholdGray: public Component {

public:
	//! Constructor.
	ComponentThresholdGray(SwisTrackCore * stc);
	//! Destructor.
	~ComponentThresholdGray();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();

	Component * Create() {
		return new ComponentThresholdGray(mCore);
	}

private:
	int mThreshold;						//!< (configuration) Threshold value.
	bool mInvertThreshold;				//!< (configuration) Whether the threshold is inverted or not
	Display mDisplayOutput;				//!< The DisplayImage showing the output of this component.
};

#endif

