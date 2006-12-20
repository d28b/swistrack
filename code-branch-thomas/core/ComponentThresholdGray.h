#ifndef HEADER_ComponentThresholdGray
#define HEADER_ComponentThresholdGray

#include "cv.h"
#include "Component.h"

class ComponentThresholdGray: public Component {

public:
	ComponentThresholdGray(SwisTrackCore *stc);
	~ComponentThresholdGray();

	// Overwritten Component methods
	bool Start();
	bool Step();
	bool StepCleanup();
	bool Stop();

private:
	IplImage *mOutputImage;

	inline void PrepareOutputImage(IplImage *inputimage) {
		if (mOutputImage) {
			if ((mOutputImage->width==inputimage->width) && (mOutputImage->height==inputimage->height)) {return;}
			cvReleaseImage(mOutputImage);
		}
		mOutputImage = cvCreateImage(cvSize(inputimage->width, inputimage->height), 8, 1);
	}
};

#endif

