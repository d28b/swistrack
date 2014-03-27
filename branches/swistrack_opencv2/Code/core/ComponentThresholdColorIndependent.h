#ifndef HEADER_ComponentThresholdColorIndependent
#define HEADER_ComponentThresholdColorIndependent

#include <cv.h>
#include "Component.h"

//! A component that applies a constant threshold to a color image, there is an independent threshold for each channels. An "OR" function is then applied between the different channels. The output is a binary image.
class ComponentThresholdColorIndependent: public Component {

public:
	//! Constructor.
	ComponentThresholdColorIndependent(SwisTrackCore *stc);
	//! Destructor.
	~ComponentThresholdColorIndependent();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {
		return new ComponentThresholdColorIndependent(mCore);
	}

private:
	IplImage *mOutputImage;				//!< The image created by this component.
	int mBlueThreshold;					//!< (configuration) Threshold value.
	int mGreenThreshold;				//!< (configuration) Threshold value.
	int mRedThreshold;					//!< (configuration) Threshold value.
	bool mOrBool;						//!< (configuration) Whether to apply a "OR" function instead of a "AND" between the channels
	bool mInvertThreshold;				//!< (configuration) Whether the threshold is inverted or not
	IplImage* tmpImage[3];				//!< Temporary Ipl images to makes the computation.
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

