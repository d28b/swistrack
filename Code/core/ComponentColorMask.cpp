#include "ComponentColorMask.h"
#define THISCLASS ComponentColorMask

#include <opencv2/highgui.hpp>
#include "DisplayEditor.h"
#include "ImageTools.h"

THISCLASS::ComponentColorMask(SwisTrackCore * stc):
	Component(stc, wxT("ColorMask")),
	mMaskImage(), mMode(cMode_BlackBlack),
	mDisplayOutput(wxT("Output"), wxT("After applying mask")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryProcessingColor);
	AddDataStructureRead(&(mCore->mDataStructureImageColor));
	AddDataStructureWrite(&(mCore->mDataStructureImageColor));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentColorMask() {
}

void THISCLASS::OnStart() {
	// Load mask image
	mMaskImage = LoadConfigurationColorImage(wxT("MaskImage"), wxT("Mask image"));

	// Mask mode
	wxString mode = GetConfigurationString(wxT("Mode"), wxT("black-black"));
	if (mode == wxT("white-white")) {
		mMode = cMode_WhiteWhite;
	} else if (mode == wxT("white-black")) {
		mMode = cMode_WhiteBlack;
		mMaskImage = ~mMaskImage;
	} else if (mode == wxT("black-white")) {
		mMode = cMode_BlackWhite;
		mMaskImage = ~mMaskImage;
	} else {
		mMode = cMode_BlackBlack;
	}

	// Load other parameters
	OnReloadConfiguration();
}

void THISCLASS::OnReloadConfiguration() {
}

void THISCLASS::OnStep() {
	cv::Mat inputImage = mCore->mDataStructureImageColor.mImage;
	if (inputImage.empty()) {
		AddError(wxT("No input image."));
		return;
	}

	// Check the size
	if (! ImageTools::EqualSize(inputImage, mMaskImage)) {
		AddError(wxT("Wrong mask size."));
		return;
	}

	// Mask the image
	cv::Mat outputImage =
	    mMode == cMode_WhiteWhite || mMode == cMode_BlackWhite ?
	    inputImage | mMaskImage :
	    inputImage & mMaskImage;

	mCore->mDataStructureImageColor.mImage = outputImage;

	// Set the display
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) de.SetMainImage(outputImage);
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
	mMaskImage.release();
}
