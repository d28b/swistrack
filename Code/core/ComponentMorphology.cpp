#include "ComponentMorphology.h"
#define THISCLASS ComponentMorphology

#include "DisplayEditor.h"
#include <algorithm>

THISCLASS::ComponentMorphology(SwisTrackCore * stc):
	Component(stc, wxT("Morphology")),
	mDisplayOutput(wxT("Output"), wxT("Morphology")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryPreprocessingBinary);
	AddDataStructureRead(&(mCore->mDataStructureImageBinary));
	AddDataStructureWrite(&(mCore->mDataStructureImageBinary));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentMorphology() {
}

void THISCLASS::OnStart()  {
	OnReloadConfiguration();
}

void THISCLASS::OnReloadConfiguration()  {
	mOperation = GetConfigurationInt(wxT("Operation"), 1);

	int iterations = GetConfigurationInt(wxT("Iterations"), 1);
	mKernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(iterations * 2 + 1, iterations * 2 + 1));
}

void THISCLASS::OnStep()  {
	// Get and check input image
	cv::Mat inputImage = mCore->mDataStructureImageBinary.mImage;
	if (inputImage.empty()) {
		AddError(wxT("No input image."));
		return;
	}

	cv::Mat outputImage(inputImage.size(), CV_8UC1);

	if (mOperation == 0) {
		// OPEN
		// dst = dilate(erode(src, element), element)
		cv::Mat erodedImage(inputImage.size(), CV_8UC1);
		cv::erode(inputImage, erodedImage, mKernel);
		cv::dilate(erodedImage, outputImage, mKernel);
	} else if (mOperation == 1) {
		// CLOSE
		// dst = erode(dilate(src, element), element)
		cv::Mat dilatedImage(inputImage.size(), CV_8UC1);
		cv::dilate(inputImage, dilatedImage, mKernel);
		cv::erode(dilatedImage, outputImage, mKernel);
	} else if (mOperation == 2) {
		// GRADIENT
		// dst = dilate(src, element) - erode(src, element)
		cv::Mat dilatedImage(inputImage.size(), CV_8UC1);
		cv::Mat erodedImage(inputImage.size(), CV_8UC1);
		cv::dilate(inputImage, dilatedImage, mKernel);
		cv::erode(inputImage, erodedImage, mKernel);
		outputImage = dilatedImage - erodedImage;
	} else if (mOperation == 3) {
		// TOPHAT
		// dst = src - open(src, element)
		cv::Mat dilatedImage(inputImage.size(), CV_8UC1);
		cv::Mat erodedImage(inputImage.size(), CV_8UC1);
		cv::erode(inputImage, erodedImage, mKernel);
		cv::dilate(erodedImage, dilatedImage, mKernel);
		outputImage = inputImage - dilatedImage;
	} else if (mOperation == 4) {
		// BLACKHAT
		// dst = close(src, element) - src
		cv::Mat dilatedImage(inputImage.size(), CV_8UC1);
		cv::Mat erodedImage(inputImage.size(), CV_8UC1);
		cv::dilate(inputImage, dilatedImage, mKernel);
		cv::erode(dilatedImage, erodedImage, mKernel);
		outputImage = erodedImage - inputImage;
	}

	mCore->mDataStructureImageBinary.mImage = outputImage;

	// Let the DisplayImage know about our image
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) de.SetMainImage(outputImage);
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
}

