#ifndef HEADER_ComponentConvertBayerToColor
#define HEADER_ComponentConvertBayerToColor

#include <cv.h>
#include "Component.h"

//! A component that converts the input image (any type) to a grayscale image.
class ComponentConvertBayerToColor: public Component {

public:
	//! Constructor.
	ComponentConvertBayerToColor(SwisTrackCore *stc);
	//! Destructor.
	~ComponentConvertBayerToColor();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {
		return new ComponentConvertBayerToColor(mCore);
	}

private:
	int mBayerType;						//!< (configuration) The Bayer Type of the camera
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

