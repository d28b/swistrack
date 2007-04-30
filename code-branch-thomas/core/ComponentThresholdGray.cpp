#include "ComponentThresholdGray.h"
#define THISCLASS ComponentThresholdGray

#include <sstream>

THISCLASS::ComponentThresholdGray(SwisTrackCore *stc):
		Component(stc, "ThresholdGray"),
		mOutputImage(0), mThreshold(128),
		mDisplayImageOutput("Output", "After thresholding") {

	// Data structure relations
	mDisplayName="Thresholding (grayscale)";
	mCategory=&(mCore->mCategoryThresholding);
	AddDataStructureRead(&(mCore->mDataStructureImageGray));
	AddDataStructureWrite(&(mCore->mDataStructureImageBinary));
	AddDisplayImage(&mDisplayImageOutput);
}

THISCLASS::~ComponentThresholdGray() {
}

void THISCLASS::OnStart() {
	OnReloadConfiguration();
}

void THISCLASS::OnReloadConfiguration() {
	mThreshold=GetConfigurationInt("Threshold", 128);
	mInvertThreshold=GetConfigurationBool("InvertThreshold", true);
}

void THISCLASS::OnStep() {
	IplImage *inputimage=mCore->mDataStructureImageGray.mImage;
	if (! inputimage) {return;}

	try {
		PrepareOutputImage(inputimage);
		if (mInvertThreshold)
			cvThreshold(inputimage, mOutputImage, mThreshold, 255, CV_THRESH_BINARY_INV);
		else
			cvThreshold(inputimage, mOutputImage, mThreshold, 255, CV_THRESH_BINARY);
		mCore->mDataStructureImageBinary.mImage=mOutputImage;
	} catch (...) {
		AddError("Thresholding failed.");
	}

	// Let the DisplayImage know about our image
	mDisplayImageOutput.mImage=mOutputImage;
	std::ostringstream oss;
	oss << "Binary image, " << mOutputImage->width << "x" << mOutputImage->height;
	mDisplayImageOutput.mAnnotation1=oss.str();
}

void THISCLASS::OnStepCleanup() {
	mCore->mDataStructureImageBinary.mImage=0;
}

void THISCLASS::OnStop() {
	if (mOutputImage) {cvReleaseImage(&mOutputImage);}
}
