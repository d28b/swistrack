#ifndef HEADER_ComponentThresholdColorCommon
#define HEADER_ComponentThresholdColorCommon

#include "cv.h"
#include "Component.h"
#include "DisplayImageStandard.h"

//! A component that applies a constant threshold to a color image, there is only one threshold for all channels.  The output is a binary image.
class ComponentThresholdColorCommon: public Component {

public:
	//! Constructor.
	ComponentThresholdColorCommon(SwisTrackCore *stc);
	//! Destructor.
	~ComponentThresholdColorCommon();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {return new ComponentThresholdColorCommon(mCore);}

private:
	IplImage *mOutputImage;						//!< The image created by this component.
	DisplayImageStandard mDisplayImageOutput;	//!< The DisplayImage showing the output of this component.
	int mThreshold;								//!< (configuration) Threshold value.
	bool mAverageBool;							//!< (configuration) Whether to apply a threshold average of the 3 channels or on each channel individually and making a "or" between the channels
	IplImage* tmpImage[3];						//!< Temporary Ipl images to makes the computation.
	
	// Prepares the output image (recreates the image if necessary).
	inline void PrepareOutputImage(IplImage *inputimage) {
		if (mOutputImage) {
			if ((mOutputImage->width==inputimage->width) && (mOutputImage->height==inputimage->height)) {return;}
			cvReleaseImage(&mOutputImage);
		}
		mOutputImage = cvCreateImage(cvSize(inputimage->width, inputimage->height), 8, 1);
	}
};

#endif
