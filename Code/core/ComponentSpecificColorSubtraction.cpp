#include "ComponentSpecificColorSubtraction.h"
#define THISCLASS ComponentSpecificColorSubtraction

#include <opencv2/highgui.hpp>
#include "DisplayEditor.h"
#include "ImageTools.h"

THISCLASS::ComponentSpecificColorSubtraction(SwisTrackCore * stc):
	Component(stc, wxT("SpecificColorSubtraction")),
	mColor(cv::Scalar(0, 0, 0)),
	mDisplayOutput(wxT("Output"), wxT("After background subtraction")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryProcessingColor);
	AddDataStructureRead(&(mCore->mDataStructureImageColor));
	AddDataStructureWrite(&(mCore->mDataStructureImageColor));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentSpecificColorSubtraction() {
}

void THISCLASS::OnStart() {
	OnReloadConfiguration();
}

void THISCLASS::OnReloadConfiguration() {
	mColor = GetConfigurationColor(wxT("Color"), cv::Scalar(0, 0, 0));

	wxString modeString = GetConfigurationString(wxT("Mode"), wxT("AbsDiff"));
	mMode =
	    modeString == wxT("ImageMinusColor") ? ImageMinusColor :
	    modeString == wxT("ColorMinusImage") ? ColorMinusImage :
	    AbsDiff;
}

void THISCLASS::OnStep() {
	cv::Mat inputImage = mCore->mDataStructureImageColor.mImage;
	if (inputImage.empty()) {
		AddError(wxT("No input image."));
		return;
	}

	cv::Mat outputImage =
	    mMode == ImageMinusColor ? inputImage - mColor :
	    mMode == ColorMinusImage ? mColor - inputImage :
	    ImageTools::AbsDiff3(inputImage, mColor);

	mCore->mDataStructureImageColor.mImage = outputImage;

	// Set the display
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) de.SetMainImage(outputImage);
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
}


