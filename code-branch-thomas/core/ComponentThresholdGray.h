#ifndef HEADER_ComponentThresholdGray
#define HEADER_ComponentThresholdGray

#include "cv.h"
#include "Component.h"

class ComponentThresholdGray: public Component {

public:
	ComponentThresholdGray(SwisTrackCore *stc);
	~ComponentThresholdGray();

	// Overwritten Component methods
	void OnStart();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {return new ComponentThresholdGray(mCore);}

private:
	IplImage *mOutputImage;
	int mThreshold;

	inline void PrepareOutputImage(IplImage *inputimage) {
		if (mOutputImage) {
			if ((mOutputImage->width==inputimage->width) && (mOutputImage->height==inputimage->height)) {return;}
			cvReleaseImage(&mOutputImage);
		}
		mOutputImage = cvCreateImage(cvSize(inputimage->width, inputimage->height), 8, 1);
	}
};

#endif

