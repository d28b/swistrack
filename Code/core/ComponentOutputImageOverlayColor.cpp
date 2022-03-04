#include "ComponentOutputImageOverlayColor.h"
#define THISCLASS ComponentOutputImageOverlayColor

#include <opencv2/highgui.hpp>
#include "DisplayEditor.h"

THISCLASS::ComponentOutputImageOverlayColor(SwisTrackCore * stc):
	Component(stc, wxT("OutputImageOverlayColor")),
	mFinalImage(), mMode(Addition), mFileName(""),
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
	mFinalImage.release();
	OnReloadConfiguration();
}

void THISCLASS::OnReloadConfiguration() {
	// Filename
	mFileName = mCore->GetRunFileName(GetConfigurationString(wxT("Filename"), wxT(""))).GetFullPath().ToStdString();

	// Mode
	wxString modeString = GetConfigurationString(wxT("Mode"), wxT("addition"));
	mMode =
	    modeString == wxT("subtraction") ? Subtraction :
	    modeString == wxT("multiplication") ? Multiplication :
	    Addition;

	// Whether to reset the image
	if (GetConfigurationButton(wxT("ResetImage")))
		mFinalImage.release();
}

void THISCLASS::OnStep() {
	// Get and check input image
	cv::Mat inputImage = mCore->mDataStructureImageColor.mImage;
	if (inputImage.empty()) {
		AddError(wxT("No input image."));
		return;
	}

	// Check and update the background
	if (mFinalImage.empty()) {
		mFinalImage = inputImage;
	} else if (mMode == Addition) {
		mFinalImage = mFinalImage + inputImage;
	} else if (mMode == Subtraction) {
		mFinalImage = mFinalImage - inputImage;
	} else if (mMode == Multiplication) {
		cv::multiply(mFinalImage, inputImage, mFinalImage);
	}

	// Set the display
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) de.SetMainImage(mFinalImage);
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
	if (mFinalImage.empty()) return;
	cv::imwrite(mFileName, mFinalImage);
	mFinalImage.release();
}
