#include "ComponentOutputImageOverlayColor.h"
#define THISCLASS ComponentOutputImageOverlayColor

#include <highgui.h>
#include "DisplayEditor.h"

THISCLASS::ComponentOutputImageOverlayColor(SwisTrackCore *stc):
		Component(stc, wxT("OutputImageOverlayColor")),
		mFinalImage(0), mMode(sMode_Addition),
		mDisplayOutput(wxT("Output"), wxT("Image overlay")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryOutput);
	AddDataStructureRead(&(mCore->mDataStructureImageColor));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentOutputImageOverlayColor() {
}

void THISCLASS::OnStart() {
	OnReloadConfiguration();

	// Make sure the final image is empty
	if (mFinalImage != NULL) {
		cvReleaseImage(&mFinalImage);
	}
	mFinalImage = NULL;
}

void THISCLASS::OnReloadConfiguration() {
	// Filename
	wxString filename_string = GetConfigurationString(wxT("Filename"), wxT(""));
	mFileName = mCore->GetRunFileName(filename_string);

	// Mode
	wxString modestr = GetConfigurationString(wxT("Mode"), wxT("addition"));
	if (modestr == wxT("subtraction")) {
		mMode = sMode_Subtraction;
	} else if (modestr == wxT("multiplication")) {
		mMode = sMode_Multiplication;
	} else {
		mMode = sMode_Addition;
	}

	// Whether to reset the image
	if (GetConfigurationBool(wxT("ResetImage"), false)) {
		cvReleaseImage(&mFinalImage);
		mFinalImage = NULL;
		mConfiguration[wxT("ResetImage")] = wxT("false");
	}
}

void THISCLASS::OnStep() {
	// Get and check input image
	IplImage *inputimage = mCore->mDataStructureImageColor.mImage;
	if (! inputimage) {
		AddError(wxT("No input image."));
		return;
	}
	if (inputimage->nChannels != 3) {
		AddError(wxT("The input image is not a color image."));
		return;
	}

	// Check and update the background
	if (! mFinalImage) {
		mFinalImage = cvCloneImage(inputimage);
	} else if (mMode == sMode_Addition) {
		cvAdd(mFinalImage, inputimage, mFinalImage);
	} else if (mMode == sMode_Subtraction) {
		cvSub(mFinalImage, inputimage, mFinalImage);
	} else if (mMode == sMode_Multiplication) {
	}

	// Set the display
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) {
		de.SetMainImage(mFinalImage);
	}
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
	if (mFinalImage) {
		cvSaveImage(mFileName.GetFullPath().mb_str(wxConvFile), mFinalImage);
		cvReleaseImage(&mFinalImage);
	}
}
