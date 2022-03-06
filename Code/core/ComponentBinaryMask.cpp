#include "ComponentBinaryMask.h"
#define THISCLASS ComponentBinaryMask

#include <opencv2/core.hpp>
#include "DisplayEditor.h"
#include "ImageTools.h"

THISCLASS::ComponentBinaryMask(SwisTrackCore * stc):
	Component(stc, wxT("BinaryMask")),
	mMaskImage(),
	mDisplayOutput(wxT("Output"), wxT("After applying mask")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryProcessingBinary);
	AddDataStructureRead(&(mCore->mDataStructureImageBinary));
	AddDataStructureWrite(&(mCore->mDataStructureImageBinary));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentBinaryMask() {
}

void THISCLASS::OnStart() {
	// Load mask image
	mMaskImage = LoadConfigurationGrayscaleImage(wxT("MaskImage"), wxT("Mask image"));

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
	cv::Mat inputImage = mCore->mDataStructureImageBinary.mImage;
	if (inputImage.empty()) {
		AddError(wxT("No input image."));
		return;
	}

	// Check the image size
	if (! ImageTools::EqualSize(inputImage, mMaskImage)) {
		AddError(wxT("Wrong mask size."));
		return;
	}

	// Mask the image
	cv::Mat outputImage =
	    mMode == cMode_WhiteWhite || mMode == cMode_BlackWhite ?
	    inputImage | mMaskImage :
	    inputImage & mMaskImage;

	mCore->mDataStructureImageBinary.mImage = outputImage;

	// Set the display
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) de.SetMainImage(mCore->mDataStructureImageBinary.mImage);
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
	mMaskImage.release();
}
