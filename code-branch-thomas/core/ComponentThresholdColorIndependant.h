#ifndef HEADER_ComponentThresholdColorIndependant
#define HEADER_ComponentThresholdColorIndependant

#include "cv.h"
#include "Component.h"
#include "DisplayImageStandard.h"

//! A component that applies a constant threshold to a color image, there is an independant threshold for each channels. An "OR" function is then applied between the different channels. The output is a binary image. 
class ComponentThresholdColorIndependant: public Component {

public:
	//! Constructor.
	ComponentThresholdColorIndependant(SwisTrackCore *stc);
	//! Destructor.
	~ComponentThresholdColorIndependant();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {return new ComponentThresholdColorIndependant(mCore);}

private:
	IplImage *mOutputImage;						//!< The image created by this component.
	DisplayImageStandard mDisplayImageOutput;	//!< The DisplayImage showing the output of this component.
	int mBlueThreshold;							//!< (configuration) Threshold value.
	int mGreenThreshold;						//!< (configuration) Threshold value.
	int mRedThreshold;							//!< (configuration) Threshold value.
	bool mOrBool;								//!< (configuration) Whether to apply a "OR" function instead of a "AND" between the channels
	IplImage* tmpImage[3];						//!< Temporary Ipl images to makes the computation.
	
	// Prepares the output image (recreates the image if necessary).
	inline void PrepareOutputImage(IplImage *inputimage) {
		if (mOutputImage) {
			if ((mOutputImage->width==inputimage->width) && (mOutputImage->height==inputimage->height)) {return;}
			cvReleaseImage(&mOutputImage);
		}
		mOutputImage = cvCreateImage(cvSize(inputimage->width, inputimage->height), 8, 1);
	}
};

#endif

