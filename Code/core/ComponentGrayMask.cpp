#include "ComponentGrayMask.h"
#define THISCLASS ComponentGrayMask

#include <opencv2/highgui.hpp>
#include "DisplayEditor.h"
#include "ImageTools.h"

THISCLASS::ComponentGrayMask(SwisTrackCore * stc):
	Component(stc, wxT("GrayMask")),
	mMaskImage(),
	mDisplayOutput(wxT("Output"), wxT("After applying mask")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryPreprocessingGray);
	AddDataStructureRead(&(mCore->mDataStructureImageGray));
	AddDataStructureWrite(&(mCore->mDataStructureImageGray));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentGrayMask() {
}

void THISCLASS::OnStart() {
	mMaskImage = LoadConfigurationGrayscaleImage(wxT("MaskImage"), wxT(""));

	// Mask mode
	wxString modeString = GetConfigurationString(wxT("Mode"), wxT("black-black"));
	if (modeString == wxT("white-white")) {
		mMode = cMode_WhiteWhite;
	} else if (modeString == wxT("white-black")) {
		mMode = cMode_WhiteBlack;
		mMaskImage = ~mMaskImage;
	} else if (modeString == wxT("black-white")) {
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
	cv::Mat inputImage = mCore->mDataStructureImageGray.mImage;
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
	    mCore->mDataStructureImageGray.mImage | mMaskImage :
	    mCore->mDataStructureImageGray.mImage & mMaskImage;

	mCore->mDataStructureImageGray.mImage = outputImage;

	// Set the display
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) de.SetMainImage(outputImage);
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
	mMaskImage.release();
}
