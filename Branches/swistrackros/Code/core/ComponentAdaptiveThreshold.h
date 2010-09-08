#ifndef HEADER_ComponentAdaptiveThreshold
#define HEADER_ComponentAdaptiveThreshold

#include <cv.h>
#include "Component.h"

//! A component that applies two thresholds to a color image, there are two independent thresholds for each channels. An "AND" function is then applied between the different channels. The output is a binary image.
class ComponentAdaptiveThreshold: public Component {

public:
	//! Constructor.
	ComponentAdaptiveThreshold(SwisTrackCore *stc);
	//! Destructor.
	~ComponentAdaptiveThreshold();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {
		return new ComponentAdaptiveThreshold(mCore);
	}

private:
	IplImage *mThisFrame;
	IplImage *mOutputImage;				//!< The image created by this component.
	int mThreshold;
	Display mDisplayOutput;				//!< The DisplayImage showing the output of this component.

	// Prepares the output image (recreates the image if necessary).
	inline void PrepareOutputImage(IplImage *inputimage) {
		if (mOutputImage)
		{
			if ((mOutputImage->width == inputimage->width) && (mOutputImage->height == inputimage->height)) {
				return;
			}
			cvReleaseImage(&mOutputImage);
		}
		mOutputImage = cvCreateImage(cvSize(inputimage->width, inputimage->height), 8, 1);
	}
};

#endif

