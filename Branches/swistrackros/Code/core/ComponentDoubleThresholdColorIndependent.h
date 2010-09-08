#ifndef HEADER_ComponentDoubleThresholdColorIndependent
#define HEADER_ComponentDoubleThresholdColorIndependent

#include <cv.h>
#include "Component.h"

//! A component that applies two thresholds to a color image, there are two independent thresholds for each channels. An "AND" function is then applied between the different channels. The output is a binary image.
class ComponentDoubleThresholdColorIndependent: public Component {

public:
	//! Constructor.
	ComponentDoubleThresholdColorIndependent(SwisTrackCore *stc);
	//! Destructor.
	~ComponentDoubleThresholdColorIndependent();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {
		return new ComponentDoubleThresholdColorIndependent(mCore);
	}

private:
	IplImage *mOutputImage;				//!< The image created by this component.
	CvScalar mLowThreshold;				//!< (configuration) Low threshold
	CvScalar mHighThreshold;			//!< (configuration) High threshold
	char thresholdColorSeq[4];			//!< Channel sequence of the thresholds.
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

