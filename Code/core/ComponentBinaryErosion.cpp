#include "ComponentBinaryErosion.h"
#define THISCLASS ComponentBinaryErosion

#include "DisplayEditor.h"

THISCLASS::ComponentBinaryErosion(SwisTrackCore * stc):
	Component(stc, wxT("BinaryErosion")),
	mIterations(1),
	mDisplayOutput(wxT("Output"), wxT("After erosion")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryProcessingBinary);
	AddDataStructureRead(&(mCore->mDataStructureImageBinary));
	AddDataStructureWrite(&(mCore->mDataStructureImageBinary));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentBinaryErosion() {
}

void THISCLASS::OnStart() {
	OnReloadConfiguration();
}

void THISCLASS::OnReloadConfiguration() {
	mIterations = GetConfigurationInt(wxT("Iterations"), 1);

	// Check for stupid configurations
	if (mIterations < 0) {
		AddError(wxT("The number of erosions must be greater or equal to 0."));
		mIterations = 1;
	}

	mKernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(mIterations * 2 + 1, mIterations * 2 + 1));
}

void THISCLASS::OnStep() {
	cv::Mat inputImage = mCore->mDataStructureImageBinary.mImage;
	if (inputImage.empty()) {
		AddError(wxT("No input image."));
		return;
	}

	cv::Mat outputImage(inputImage.size(), CV_8UC1);
	cv::erode(inputImage, outputImage, mKernel);

	// Set the display
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) de.SetMainImage(outputImage);
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
}
