#ifndef HEADER_ComponentConvertToBGR
#define HEADER_ComponentConvertToBGR

#include "cv.h"
#include "Component.h"

//! A component that converts the input image (any format) to BGR.
class ComponentConvertToBGR: public Component {

public:
	//! Constructor.
	ComponentConvertToBGR(SwisTrackCore *stc);
	//! Destructor.
	~ComponentConvertToBGR();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {return new ComponentConvertToBGR(mCore);}

private:
	IplImage *mOutputImage;		//!< The image created by this component.

	// Prepares the output image (recreates the image if necessary).
	inline void PrepareOutputImage(IplImage *inputimage) {
		if (mOutputImage) {
			if ((mOutputImage->width==inputimage->width) && (mOutputImage->height==inputimage->height) && (mOutputImage->depth==inputimage->depth)) {return;}
			cvReleaseImage(&mOutputImage);
		}
		mOutputImage = cvCreateImage(cvSize(inputimage->width, inputimage->height), inputimage->depth, 1);
	}
};

#endif

