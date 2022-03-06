#include "ComponentSobelDifferentiation.h"
#define THISCLASS ComponentSobelDifferentiation

#include "DisplayEditor.h"
#include <algorithm>

THISCLASS::ComponentSobelDifferentiation(SwisTrackCore * stc):
	Component(stc, wxT("SobelDifferentiation")),
	mDisplayOutput(wxT("Output"), wxT("Differentiation")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryProcessingGray);
	AddDataStructureRead(&(mCore->mDataStructureImageGray));
	AddDataStructureWrite(&(mCore->mDataStructureImageGray));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentSobelDifferentiation() {
}

void THISCLASS::OnStart() {
	OnReloadConfiguration();
}

void THISCLASS::OnReloadConfiguration() {
	mOrderX = GetConfigurationInt(wxT("OrderX"), 1);
	mOrderY = GetConfigurationInt(wxT("OrderY"), 1);
	mKernelSize = GetConfigurationInt(wxT("KernelSize"), 3);

	if (mOrderX < 0) mOrderX = 0;
	if (mOrderX > 3) mOrderX = 3;
	if (mOrderY < 0) mOrderY = 0;
	if (mOrderY > 3) mOrderY = 3;

	// Make sure the differentiation order is less than the kernel size
	if (mKernelSize == 1 || mKernelSize == 3) {
		if (mOrderX > 2) mOrderX = 2;
		if (mOrderY > 2) mOrderY = 2;
	}
}

void THISCLASS::OnStep() {
	// Get and check input image
	cv::Mat inputImage = mCore->mDataStructureImageGray.mImage;
	if (inputImage.empty()) {
		AddError(wxT("No input image."));
		return;
	}

	cv::Mat outputImage(inputImage.rows, inputImage.cols, CV_8UC1);
	cv::Sobel(inputImage, outputImage, -1, mOrderX, mOrderY, mKernelSize);
	mCore->mDataStructureImageGray.mImage = outputImage;

	// Let the DisplayImage know about our image
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) de.SetMainImage(outputImage);
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
}

