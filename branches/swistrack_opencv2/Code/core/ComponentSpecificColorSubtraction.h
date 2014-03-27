#ifndef HEADER_ComponentSpecificColorSubtraction
#define HEADER_ComponentSpecificColorSubtraction

#include <cv.h>
#include "Component.h"

//! A component subtracting a fixed background from a color image.
class ComponentSpecificColorSubtraction: public Component {

public:
	//! Constructor.
	ComponentSpecificColorSubtraction(SwisTrackCore *stc);
	//! Destructor.
	~ComponentSpecificColorSubtraction();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {
		return new ComponentSpecificColorSubtraction(mCore);
	}

private:
	bool mCorrectMean;					//!< (configuration) Whether to correct for the mean or not.
	bool mSpecifyColorBool;				//!< Specify the color on the input image.
	CvScalar mTrackedColor;				//!< The specified color that is tracked in the image.
	char trackedColorSeq[4];			//!< Channel sequence of the tracked color.
	CvScalar mSpecifiedImageAverage;	//!< Average value of the image used to specify the color, or the first image after using this option
	Display mDisplayOutput;				//!< The DisplayImage showing the output of this component.
};

#endif

