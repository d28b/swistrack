#ifndef HEADER_ComponentThresholdGray
#define HEADER_ComponentThresholdGray

#include <cv.h>
#include "Component.h"

//! A component that applies a constant threshold to a grayscale image. The output is a binary image.
class ComponentThresholdGray: public Component {

public:
	//! Constructor.
	ComponentThresholdGray(SwisTrackCore *stc);
	//! Destructor.
	~ComponentThresholdGray();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {
		return new ComponentThresholdGray(mCore);
	}

private:
	IplImage *mOutputImage;				//!< The image created by this component.
	int mThreshold;						//!< (configuration) Threshold value.
	bool mInvertThreshold;				//!< (configuration) Whether the threshold is inverted or not
	Display mDisplayOutput;				//!< The DisplayImage showing the output of this component.

	// Prepares the output image (recreates the image if necessary).
	inline void PrepareOutputImage(IplImage *inputimage) {
		if (mOutputImage) {
			if ((mOutputImage->width == inputimage->width) && (mOutputImage->height == inputimage->height)) {
				return;
			}
			cvReleaseImage(&mOutputImage);
		}
		mOutputImage = cvCreateImage(cvSize(inputimage->width, inputimage->height), 8, 1);
	}
};

#endif

