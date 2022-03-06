#include "ComponentCannyEdgeDetection.h"
#define THISCLASS ComponentCannyEdgeDetection

#include "DisplayEditor.h"

THISCLASS::ComponentCannyEdgeDetection(SwisTrackCore * stc):
	Component(stc, wxT("CannyEdgeDetection")),
	mDisplayOutput(wxT("Output"), wxT("After edge detection")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryProcessingGray);
	AddDataStructureRead(&(mCore->mDataStructureImageGray));
	AddDataStructureWrite(&(mCore->mDataStructureImageBinary));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentCannyEdgeDetection() {
}

void THISCLASS::OnStart() {
	OnReloadConfiguration();
}

void THISCLASS::OnReloadConfiguration() {
	mThreshold1 = GetConfigurationDouble(wxT("Threshold1"), 50);
	mThreshold2 = GetConfigurationDouble(wxT("Threshold2"), 200);
}

void THISCLASS::OnStep() {
	cv::Mat inputImage = mCore->mDataStructureImageGray.mImage;
	if (inputImage.empty()) {
		AddError(wxT("No input image."));
		return;
	}

	cv::Mat outputImage(inputImage.size(), CV_8UC3);
	cv::Canny(inputImage, outputImage, mThreshold1, mThreshold2);
	mCore->mDataStructureImageBinary.mImage = outputImage;

	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) de.SetMainImage(outputImage);
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
}
