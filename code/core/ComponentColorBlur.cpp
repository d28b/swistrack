#include "ComponentColorBlur.h"
#define THISCLASS ComponentColorBlur

#include <highgui.h>
#include <sstream>
#include <cctype>
#include <algorithm>
#include "DisplayEditor.h"

THISCLASS::ComponentColorBlur(SwisTrackCore *stc):
		Component(stc, "ColorBlur"),
		mBlurType(CV_GAUSSIAN), mRadius(1),
		mDisplayOutput("Output", "After blurring the image") {

	// Data structure relations
	mCategory=&(mCore->mCategoryPreprocessingColor);
	AddDataStructureRead(&(mCore->mDataStructureImageColor));
	AddDataStructureWrite(&(mCore->mDataStructureImageColor));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentColorBlur() {
}

void THISCLASS::OnStart() {
	OnReloadConfiguration();
	return;
}

void THISCLASS::OnReloadConfiguration() {
	// Type
	std::string str=GetConfigurationString("Type", "gaussian");
	std::transform(str.begin(), str.end(), str.begin(), (int(*)(int))std::tolower);
	if (str=="sum") {
		mBlurType=CV_BLUR_NO_SCALE;
	} else if (str=="mean") {
		mBlurType=CV_BLUR;
	} else if (str=="median") {
		mBlurType=CV_MEDIAN;
	} else {
		mBlurType=CV_GAUSSIAN;
	}

	// Radius
	mRadius=GetConfigurationInt("Radius", 1);
	if (mRadius<0) {mRadius=0;}
	if (mRadius % 2 == 0) {
		mRadius++;
		AddWarning("Radius must be an odd number. Using radius+1 for the blur.");
	}
}

void THISCLASS::OnStep() {
	if (! mCore->mDataStructureImageColor.mImage) {
		AddError("No input image.");
		return;
	}

	cvSmooth(mCore->mDataStructureImageColor.mImage, mCore->mDataStructureImageColor.mImage, mBlurType, mRadius, mRadius);

	// Set the display
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) {
		de.SetMainImage(mCore->mDataStructureImageColor.mImage);
	}
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
	
}
