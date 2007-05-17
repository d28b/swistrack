#ifndef HEADER_ComponentBackgroundSubtractionGray
#define HEADER_ComponentBackgroundSubtractionGray

#include <cv.h>
#include "Component.h"

//! A component subtracting a fixed background from a grayscale image.
class ComponentBackgroundSubtractionGray: public Component {

public:
	//! Constructor.
	ComponentBackgroundSubtractionGray(SwisTrackCore *stc);
	//! Destructor.
	~ComponentBackgroundSubtractionGray();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {return new ComponentBackgroundSubtractionGray(mCore);}

private:
	IplImage *mBackgroundImage;			//!< The background image.
	CvScalar mBackgroundImageMean;		//!< The mean of the background image.
	bool mCorrectMean;					//!< (configuration) Whether to correct for the mean or not.

	Display mDisplayOutput;				//!< The DisplayImage showing the output of this component.
};

#endif

