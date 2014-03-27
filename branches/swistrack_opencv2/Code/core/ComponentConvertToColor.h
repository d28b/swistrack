#ifndef HEADER_ComponentConvertToColor
#define HEADER_ComponentConvertToColor

#include <cv.h>
#include "Component.h"

//! A component that converts the input image (any format) to BGR.
class ComponentConvertToColor: public Component {

public:
	//! Constructor.
	ComponentConvertToColor(SwisTrackCore *stc);
	//! Destructor.
	~ComponentConvertToColor();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {
		return new ComponentConvertToColor(mCore);
	}

private:
	IplImage *mOutputImage;				//!< The image created by this component.
	Display mDisplayOutput;				//!< The DisplayImage showing the output of this component.

	//! Prepares the output image (recreates the image if necessary).
	inline void PrepareOutputImage(IplImage *inputimage) {
		if (mOutputImage) {
			if ((mOutputImage->width == inputimage->width) && (mOutputImage->height == inputimage->height) && (mOutputImage->depth == inputimage->depth)) {
				return;
			}
			cvReleaseImage(&mOutputImage);
		}
		mOutputImage = cvCreateImage(cvSize(inputimage->width, inputimage->height), inputimage->depth, 3);
	}
};

#endif

