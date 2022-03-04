#ifndef HEADER_ComponentCorrectMean
#define HEADER_ComponentCorrectMean

#include <opencv2/core.hpp>
#include "Component.h"

//! A component subtracting a fixed background from a color image.
class ComponentCorrectMean: public Component {

public:
	//! Constructor.
	ComponentCorrectMean(SwisTrackCore * stc);
	//! Destructor.
	~ComponentCorrectMean();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();

	Component * Create() {
		return new ComponentCorrectMean(mCore);
	}

private:
	cv::Scalar mColor;				//!< (configuration) The specified color to subtract.
	Display mDisplayOutput;			//!< The DisplayImage showing the output of this component.
};

#endif

