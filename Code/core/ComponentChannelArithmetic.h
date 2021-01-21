#ifndef HEADER_ComponentChannelArithmetic
#define HEADER_ComponentChannelArithmetic

#include <cv.h>
#include "Component.h"

typedef enum _OP {
	ADD = 0,
	SUB = 1,
	MUL = 2,
	DIV = 3,
} OP;

//! A component subtracting a fixed background from a color image.
class ComponentChannelArithmetic: public Component {

public:
	//! Constructor.
	ComponentChannelArithmetic(SwisTrackCore *stc);
	//! Destructor.
	~ComponentChannelArithmetic();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {
		return new ComponentChannelArithmetic(mCore);
	}

private:
	double coeff1;
	double coeff2;
	double coeff3;
	int channel1;
	int channel2;
	int channel3;
	OP op1;
	OP op2;

	char channelColorSeq[4];            //!< The sequence of channel used for the channel

	IplImage *mOutputImage;             //!< The image created by this component.
	Display mDisplayOutput;				//!< The DisplayImage showing the output of this component.

	inline void PrepareOutputImage(IplImage *inputimage) {
		if (mOutputImage) {
			if ((mOutputImage->width == inputimage->width) && (mOutputImage->height == inputimage->height) && (mOutputImage->depth == inputimage->depth)) {
				return;
			}
			cvReleaseImage(&mOutputImage);
		}
		mOutputImage = cvCreateImage(cvSize(inputimage->width, inputimage->height), inputimage->depth, 1);
	}

};

#endif

