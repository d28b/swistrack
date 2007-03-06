#ifndef HEADER_ComponentConvertToGray
#define HEADER_ComponentConvertToGray

#include "cv.h"
#include "Component.h"
#include "DisplayImageStandard.h"

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
	Component *Create() {return new ComponentConvertToGray(mCore);}

private:
	IplImage *mOutputImage;						//!< The image created by this component.
	DisplayImageStandard mDisplayImageOutput;	//!< The DisplayImage showing the last acquired image.

	//! Converts from packed YUV422 to grayscale.
	void CvtYUV422ToGray(IplImage *inputimage, IplImage *outputimage);

	//! Prepares the output image (recreates the image if necessary).
	inline void PrepareOutputImage(IplImage *inputimage) {
		if (mOutputImage) {
			if ((mOutputImage->width==inputimage->width) && (mOutputImage->height==inputimage->height) && (mOutputImage->depth==inputimage->depth)) {return;}
			cvReleaseImage(&mOutputImage);
		}
		mOutputImage = cvCreateImage(cvSize(inputimage->width, inputimage->height), inputimage->depth, 1);
	}
};

#endif

