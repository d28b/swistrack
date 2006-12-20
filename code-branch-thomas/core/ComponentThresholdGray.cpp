#include "ComponentThresholdGray.h"
#define THISCLASS ComponentThresholdGray

THISCLASS::ComponentThresholdGray(SwisTrackCore *stc):
		Component(stc, "ThresholdGray"),
		mCapture(0), mLastImage(0) {

	// Data structure relations
	mDisplayName="Thresholding (grayscale)";
	mCategory="Preprocessing (grayscale)";
	AddDataStructureWrite(mCore->mDataStructureImage);
	AddDataStructureWrite(mCore->mDataStructureImageBinary);
}

THISCLASS::~ComponentThresholdGray() {
	if (! mCapture) {return;}
	cvReleaseCapture(mCapture);
}

bool THISCLASS::Start() {
	return true;
}

bool THISCLASS::Step() {
	IplImage *inputimage=mCore->mDataStructureImageGray.mImage;
	if (! inputimage) {return true;}

	try {
		PrepareOutputImage(inputimage);
		cvThreshold(inputimage, binary, mThreshold, 255, CV_THRESH_BINARY);
	} catch (...) {
		AddError("Thresholding failed.");
	}
	
	cvReleaseImage(&tmpImage);
	return true;
}

bool THISCLASS::Stop() {
	if (mOutputImage) {cvReleaseCapture(mOutputImage);}
	return true;
}
