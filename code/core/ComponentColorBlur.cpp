#include "ComponentColorBlur.h"
#define THISCLASS ComponentColorBlur

#include <highgui.h>
#include <cctype>
#include <algorithm>
#include "DisplayEditor.h"

THISCLASS::ComponentColorBlur(SwisTrackCore *stc):
		Component(stc, wxT("ColorBlur")),
		mBlurType(CV_GAUSSIAN), mRadius(1),
		mDisplayOutput(wxT("Output"), wxT("After blurring the image")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryPreprocessingColor);
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
	wxString str = GetConfigurationString(wxT("Type"), wxT("gaussian"));
	str.MakeLower();
	if (str == wxT("sum")) {
		mBlurType = CV_BLUR_NO_SCALE;
	} else if (str == wxT("mean")) {
		mBlurType = CV_BLUR;
	} else if (str == wxT("median")) {
		mBlurType = CV_MEDIAN;
	} else {
		mBlurType = CV_GAUSSIAN;
	}

	// Radius
	mRadius = GetConfigurationInt(wxT("Radius"), 1);
	if (mRadius < 0) {
		mRadius = 0;
	}
	if (mRadius % 2 == 0) {
		mRadius++;
		AddWarning(wxT("Radius must be an odd number. Using radius+1 for the blur."));
	}
}

void THISCLASS::OnStep() {
	if (! mCore->mDataStructureImageColor.mImage) {
		AddError(wxT("No input image."));
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
