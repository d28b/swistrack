#include "ComponentColorBlur.h"
#define THISCLASS ComponentColorBlur

#include <highgui.h>
#include <sstream>
#include "DisplayEditor.h"

THISCLASS::ComponentColorBlur(SwisTrackCore *stc):
		Component(stc, "ColorBlur"),
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
	mRadius=GetConfigurationInt("Radius", 1);
}

void THISCLASS::OnStep() {
	if (! mCore->mDataStructureImageColor.mImage) {
		AddError("No input image.");
		return;
	}

	if (mRadius) {
		cvSmooth(mCore->mDataStructureImageColor.mImage, mCore->mDataStructureImageColor.mImage, CV_GAUSSIAN, mRadius);
	}

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
