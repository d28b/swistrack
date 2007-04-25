#ifndef HEADER_ComponentBackgroundSubtractionColor
#define HEADER_ComponentBackgroundSubtractionColor

#include "cv.h"
#include "Component.h"
#include "DisplayImageStandard.h"

//! A component subtracting a fixed background from a color image.
class ComponentBackgroundSubtractionColor: public Component {

public:
	//! Constructor.
	ComponentBackgroundSubtractionColor(SwisTrackCore *stc);
	//! Destructor.
	~ComponentBackgroundSubtractionColor();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {return new ComponentBackgroundSubtractionColor(mCore);}

private:
	IplImage *mBackgroundImage;		//!< The background image.
	CvScalar mBackgroundImageMean;	//!< The mean of the background image.
	bool mCorrectMean;				//!< (configuration) Whether to correct for the mean or not.

	DisplayImageStandard mDisplayImageOutput;	//!< The DisplayImage showing the last processed image.
};

#endif

