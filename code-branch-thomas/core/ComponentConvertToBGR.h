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
	IplImage *mOutputImage;				//!< The image created by this component.
	IplImage *mOutputImage0;			//!< The image created by this component.
	IplImage *mOutputImage1;			//!< The image created by this component.
	IplImage *mOutputImage2;			//!< The image created by this component.
	Display mDisplayOutput;				//!< The DisplayImage showing the output of this component.

	//! Converts from packed YUV422 to BGR.
	void CvtYUV422ToBGR(IplImage *inputimage, IplImage *outputimage);

	//! Prepares the output image (recreates the image if necessary).
	inline void PrepareOutputImage(IplImage *inputimage) {
		if (mOutputImage) {
			if ((mOutputImage->width==inputimage->width) && (mOutputImage->height==inputimage->height) && (mOutputImage->depth==inputimage->depth)) {return;}
			cvReleaseImage(&mOutputImage);
			cvReleaseImage(&mOutputImage0);
			cvReleaseImage(&mOutputImage1);
			cvReleaseImage(&mOutputImage2);
		}
		mOutputImage = cvCreateImage(cvSize(inputimage->width, inputimage->height), inputimage->depth, 3);
		mOutputImage0 = cvCreateImage(cvSize(inputimage->width, inputimage->height), inputimage->depth, 1);
		mOutputImage1 = cvCreateImage(cvSize(inputimage->width, inputimage->height), inputimage->depth, 1);
		mOutputImage2 = cvCreateImage(cvSize(inputimage->width, inputimage->height), inputimage->depth, 1);
	}
};

#endif

