#include "ComponentThresholdGray.h"
#define THISCLASS ComponentThresholdGray

#include "DisplayEditor.h"

THISCLASS::ComponentThresholdGray(SwisTrackCore *stc):
		Component(stc, wxT("ThresholdGray")),
		mOutputImage(0), mThreshold(128), mInvertThreshold(false),
		mDisplayOutput(wxT("Output"), wxT("After thresholding")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryThresholdingGray);
	AddDataStructureRead(&(mCore->mDataStructureImageGray));
	AddDataStructureWrite(&(mCore->mDataStructureImageBinary));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentThresholdGray() {
}

void THISCLASS::OnStart() {
	OnReloadConfiguration();
}

void THISCLASS::OnReloadConfiguration() {
	mThreshold = GetConfigurationInt(wxT("Threshold"), 128);
	mInvertThreshold = GetConfigurationBool(wxT("InvertThreshold"), false);
}

void THISCLASS::OnStep() {
	IplImage *inputimage = mCore->mDataStructureImageGray.mImage;
	if (! inputimage) {
		AddError(wxT("No input image."));
		return;
	}

	try {
		PrepareOutputImage(inputimage);
		if (mInvertThreshold)
			cvThreshold(inputimage, mOutputImage, mThreshold, 255, CV_THRESH_BINARY_INV);
		else
			cvThreshold(inputimage, mOutputImage, mThreshold, 255, CV_THRESH_BINARY);
		mCore->mDataStructureImageBinary.mImage = mOutputImage;
	} catch (...) {
		AddError(wxT("Thresholding failed."));
	}

	// Set the display
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) {
		de.SetMainImage(mOutputImage);
	}
}

void THISCLASS::OnStepCleanup() {
	mCore->mDataStructureImageBinary.mImage = 0;
}

void THISCLASS::OnStop() {
	if (mOutputImage) {
		cvReleaseImage(&mOutputImage);
	}
}
