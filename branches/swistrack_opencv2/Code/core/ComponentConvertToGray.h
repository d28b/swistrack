#ifndef HEADER_ComponentConvertToGray
#define HEADER_ComponentConvertToGray

#include <cv.h>
#include "Component.h"

//! A component that converts the input image (any type) to a grayscale image.
class ComponentConvertToGray: public Component {

public:
	//! Constructor.
	ComponentConvertToGray(SwisTrackCore *stc);
	//! Destructor.
	~ComponentConvertToGray();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {
		return new ComponentConvertToGray(mCore);
	}

private:
	IplImage *mOutputImage;				//!< The image created by this component.
	Display mDisplayOutput;				//!< The DisplayImage showing the output of this component.
	int mChannel;						//!< The number of the channel we use for the conversion
	char mChannelColorSeq[4];			//!< The sequence of channel used for the channel

	//! Prepares the output image (recreates the image if necessary).
	inline void PrepareOutputImage(IplImage *inputimage) {
		if (mOutputImage) {
			if ((mOutputImage->width == inputimage->width) && (mOutputImage->height == inputimage->height) && (mOutputImage->depth == inputimage->depth)) {
				return;
			}
			cvReleaseImage(&mOutputImage);
		}
		mOutputImage = cvCreateImage(cvSize(inputimage->width, inputimage->height), inputimage->depth, 1);
	}
};

#endif

