#ifndef HEADER_ComponentBackgroundSubtractionCheungKamath
#define HEADER_ComponentBackgroundSubtractionCheungKamath

#include <cv.h>
#include "Component.h"

//! A component subtracting a fixed background from a color image.
class ComponentBackgroundSubtractionCheungKamath: public Component {

public:
	//! Constructor.
	ComponentBackgroundSubtractionCheungKamath(SwisTrackCore *stc);
	//! Destructor.
	~ComponentBackgroundSubtractionCheungKamath();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {
		return new ComponentBackgroundSubtractionCheungKamath(mCore);
	}

private:

	
	IplImage *mOutputImage;
	IplImage *mShortTermForegroundMask;		//!< The background image.
	IplImage *mTmp;		//!< The background image.
	IplImage *mR;		//!< The background image.
	IplImage *mB;		//!< The background image.
	IplImage *mG;		//!< The background image.
	IplImage *mTmpColors;		//!< The background image.
	IplImage *mTmpBinary;		//!< The background image.
	IplImage *mLastFrame;		//!< The background image.
	CvScalar mBackgroundImageMean;	//!< The mean of the background image.
	CvMemStorage * mStorage;

	double mThreshold;						//!< (configuration) Threshold value.

	Display mDisplayOutput;			//!< The DisplayImage showing the output of this component.
};

#endif

