#ifndef HEADER_ComponentConvertToGray
#define HEADER_ComponentConvertToGray

#include "cv.h"
#include "Component.h"

class ComponentConvertToGray: public Component {

public:
	ComponentConvertToGray(SwisTrackCore *stc);
	~ComponentConvertToGray();

	// Overwritten Component methods
	void OnStart();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {return new ComponentConvertToGray(mCore);}

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

