#ifndef HEADER_ComponentHSVBackgroundSubtractionColor
#define HEADER_ComponentHSVBackgroundSubtractionColor

#include <cv.h>
#include "Component.h"

//! A component subtracting a fixed background from a color image.
class ComponentHSVBackgroundSubtractionColor: public Component {

public:
	//! Constructor.
	ComponentHSVBackgroundSubtractionColor(SwisTrackCore *stc);
	//! Destructor.
	~ComponentHSVBackgroundSubtractionColor();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {
		return new ComponentHSVBackgroundSubtractionColor(mCore);
	}

private:
	IplImage *mBackgroundImage;			//!< The background image.
	IplImage *mBackgroundImageHSV;		//!< The background image in HSV.
	IplImage* tmpHSVImage;				//!< Temporary image used for computation.
	IplImage* tmpBinaryImage;			//!< Temporary image used for computation.
	IplImage *outputImage;				//!< Image used for the output
	bool mCorrectMean;					//!< (configuration) Whether to correct for the mean or not.
	Display mDisplayOutput;				//!< The DisplayImage showing the output of this component.
};

#endif

