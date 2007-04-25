#ifndef HEADER_ComponentAdaptiveBackgroundSubtractionGray
#define HEADER_ComponentAdaptiveBackgroundSubtractionGray

#include "cv.h"
#include "Component.h"
#include "DisplayImageStandard.h"

//! A component subtracting a fixed background from a color image.
class ComponentAdaptiveBackgroundSubtractionGray: public Component {

public:
	//! Constructor.
	ComponentAdaptiveBackgroundSubtractionGray(SwisTrackCore *stc);
	//! Destructor.
	~ComponentAdaptiveBackgroundSubtractionGray();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {return new ComponentAdaptiveBackgroundSubtractionGray(mCore);}

private:
	IplImage *mBackgroundImage;		//!< The background image.
	CvScalar mBackgroundImageMean;	//!< The mean of the background image.
	bool mCorrectMean;				//!< (configuration) Whether to correct for the mean or not.
	float mUpdateProportion;		//!< (configuration) Proportion of the input image added to the background

	DisplayImageStandard mDisplayImageOutput;	//!< The DisplayImage showing the last processed image.
};

#endif

