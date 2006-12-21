#ifndef HEADER_ComponentConvertToBGR
#define HEADER_ComponentConvertToBGR

#include "cv.h"
#include "Component.h"

class ComponentConvertToBGR: public Component {

public:
	ComponentConvertToBGR(SwisTrackCore *stc);
	~ComponentConvertToBGR();

	// Overwritten Component methods
	void OnStart();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {return new ComponentConvertToBGR(mCore);}

private:
	IplImage *mOutputImage;

	inline void PrepareOutputImage(IplImage *inputimage) {
		if (mOutputImage) {
			if ((mOutputImage->width==inputimage->width) && (mOutputImage->height==inputimage->height) && (mOutputImage->depth==inputimage->depth)) {return;}
			cvReleaseImage(&mOutputImage);
		}
		mOutputImage = cvCreateImage(cvSize(inputimage->width, inputimage->height), inputimage->depth, 1);
	}
};

#endif

