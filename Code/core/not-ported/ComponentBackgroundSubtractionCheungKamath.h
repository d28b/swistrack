#ifndef HEADER_ComponentBackgroundSubtractionCheungKamath
#define HEADER_ComponentBackgroundSubtractionCheungKamath

#include <opencv2/core.hpp>
#include "Component.h"

//! A component subtracting a fixed background from a color image.
class ComponentBackgroundSubtractionCheungKamath: public Component {

public:
	//! Constructor.
	ComponentBackgroundSubtractionCheungKamath(SwisTrackCore * stc);
	//! Destructor.
	~ComponentBackgroundSubtractionCheungKamath();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();

	Component * Create() {
		return new ComponentBackgroundSubtractionCheungKamath(mCore);
	}

private:


	cv::Mat *mOutputImage;
	cv::Mat *mShortTermForegroundMask;		//!< The background image.
	cv::Mat *mTmp;		//!< The background image.
	cv::Mat *mR;		//!< The background image.
	cv::Mat *mB;		//!< The background image.
	cv::Mat *mG;		//!< The background image.
	cv::Mat *mTmpColors;		//!< The background image.
	cv::Mat *mTmpBinary;		//!< The background image.
	cv::Mat *mLastFrame;		//!< The background image.
	cv::Scalar mBackgroundImageMean;	//!< The mean of the background image.
	CvMemStorage * mStorage;

	double mThreshold;						//!< (configuration) Threshold value.

	Display mDisplayOutput;			//!< The DisplayImage showing the output of this component.
};

#endif

