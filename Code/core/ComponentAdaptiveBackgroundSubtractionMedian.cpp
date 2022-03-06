#include "ComponentAdaptiveBackgroundSubtractionMedian.h"
#define THISCLASS ComponentAdaptiveBackgroundSubtractionMedian

#include <opencv2/core.hpp>
#include "DisplayEditor.h"
#include <cstdio>

int LInfDistance(cv::Scalar p1, cv::Scalar p2) {
	int maxVal = 0;
	for (int i = 0; i < 3; i++) {
		int val = abs(p1.val[i] - p2.val[i]);
		if (val > maxVal) {
			maxVal = val;
		}
	}
	return maxVal;
}


THISCLASS::ComponentAdaptiveBackgroundSubtractionMedian(SwisTrackCore * stc):
	Component(stc, wxT("AdaptiveBackgroundSubtractionMedian")),
	mBackgroundModel(),
	mDisplayOutput(wxT("Output"), wxT("After background subtraction")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryProcessingColor);
	AddDataStructureRead(&(mCore->mDataStructureImageColor));
	AddDataStructureWrite(&(mCore->mDataStructureImageColor));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentAdaptiveBackgroundSubtractionMedian() {
}

void THISCLASS::OnStart() {
	OnReloadConfiguration();
}

void THISCLASS::OnReloadConfiguration() {

}

void THISCLASS::OnStep() {
	// Get and check input image
	cv::Mat inputImage = mCore->mDataStructureImageColor.mImage;
	if (inputImage.empty()) {
		AddError(wxT("No input image."));
		return;
	}

	if (inputImage.channels() != 3) {
		AddError(wxT("The input image is not a color image."));
		return;
	}

	if (mBackgroundModel.empty()) {
		//cv::imwrite("aaaaa.jpg", inputImage);
		mBackgroundModel = inputImage.clone();
	}

	//char filename[30];
	//sprintf(filename, "Baseline%d.jpg", mCore->mDataStructureInput.mFrameNumber);
	//cv::imwrite(filename, inputImage);

	cv::Mat outputImage = UpdateBackgroundModel(inputImage);
	mCore->mDataStructureImageColor.mImage = outputImage;

	// Set the display
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) {
		de.SetMainImage(outputImage);
		//de.SetMainImage(mBackgroundModel);
	}
}

cv::Mat THISCLASS::UpdateBackgroundModel(cv::Mat inputImage) {
	for (int y = 0; y < mBackgroundModel.rows; y++) {
		unsigned char * bgptr = mBackgroundModel.ptr(y);
		unsigned char * cptr = inputImage.ptr(y);
		for (int x = 0; x < mBackgroundModel.cols; x++) {
			for (int i = 0; i < 3; i++) {
				int bgval = bgptr[3 * x + i];
				int imgval = cptr[3 * x + i];
				if (bgval < imgval) {
					bgptr[3 * x + i] += 1;
				} else if (bgval > imgval) {
					bgptr[3 * x + i] -= 1;
				} else {
					// nothing if they are equal
				}
			}
		}
	}

	cv::Mat outputImage;
	cv::absdiff(inputImage, mBackgroundModel, outputImage);
	return outputImage;
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
}
