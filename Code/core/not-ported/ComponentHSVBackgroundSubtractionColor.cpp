#include "ComponentHSVBackgroundSubtractionColor.h"
#define THISCLASS ComponentHSVBackgroundSubtractionColor

#include <opencv2/core.hpp>
#include "DisplayEditor.h"

THISCLASS::ComponentHSVBackgroundSubtractionColor(SwisTrackCore * stc):
		Component(stc, wxT("HSVBackgroundSubtractionColor")),
		mBackgroundImage(0), mCorrectMean(true),
		mDisplayOutput(wxT("Output"), wxT("After background subtraction")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryProcessingColor);
	AddDataStructureRead(&(mCore->mDataStructureImageColor));
	AddDataStructureWrite(&(mCore->mDataStructureImageGray));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentHSVBackgroundSubtractionColor() {
}

void THISCLASS::OnStart() {
	mBackgroundImage = LoadConfigurationColorImage(wxT("BackgroundImage"), wxT("Background image"));

	cv::Mat tmpHSVImage(mBackgroundImage.size(), CV_8UC3);
	mBackgroundImageHSV = cv::Mat(mBackgroundImage.size(), CV_8UC1);

	cv::cvtColor(mBackgroundImage, tmpHSVImage, cv::COLOR_RGB2HSV);
	cv::split(tmpHSVImage, mBackgroundImageHSV, NULL, NULL, NULL);
}

void THISCLASS::OnReloadConfiguration() {
}

void THISCLASS::OnStep() {
	cv::Mat inputImage = mCore->mDataStructureImageColor.mImage;
	if (inputImage.empty()) {
		AddError(wxT("No input image."));
		return;
	}

	if (! ImageTools::EqualSize(inputImage, mBackgroundImage)) {
		AddError(wxT("Input and background images do not have the same size."));
		return;
	}

	cv::Mat outputImage(inputImage.size(), CV_8UC1);

	cv::cvtColor(inputImage, tmpHSVImage, cv::COLOR_RGB2HSV);
	cvSplit(tmpHSVImage, outputImage, NULL, NULL, NULL);
	// Background Substraction
	cvAbsDiff(outputImage, mBackgroundImageHSV, outputImage);
	//Here we have to do something fancy, has in HSV 180 is close to 0
	cvThreshold(outputImage, tmpBinaryImage, 90, 255, CV_THRESH_BINARY);
	cvSubRS(outputImage, cvScalar(180), outputImage, tmpBinaryImage);

	mCore->mDataStructureImageGray.mImage = outputImage;

	// Set the display
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) de.SetMainImage(outputImage);
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
}
