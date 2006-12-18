#ifndef HEADER_ComponentConvertToGray
#define HEADER_ComponentConvertToGray

#include "cv.h"
#include "Component.h"

class ComponentConvertToGray: public Component {

public:
	ComponentConvertToGray(SwisTrackCore *stc);
	~ComponentConvertToGray();

	// Overwritten Component methods
	bool Start();
	bool Step();
	bool Stop();

private:
	IplImage *mImage;

	inline void PrepareImage(IplImage *inputimage) {
		if (mImage) {
			if ((mImage->width==inputimage->width) && (mImage->height==inputimage->height) && (mImage->depth==inputimage->depth)) {return;}
			cvReleaseImage(mImage);
		}
		mImage = cvCreateImage(cvSize(inputimage->width, inputimage->height), inputimage->depth, 1);
	}
};

#endif

