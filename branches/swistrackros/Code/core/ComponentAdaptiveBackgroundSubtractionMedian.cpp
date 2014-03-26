#include "ComponentAdaptiveBackgroundSubtractionMedian.h"
#define THISCLASS ComponentAdaptiveBackgroundSubtractionMedian

#include <highgui.h>
#include "DisplayEditor.h"
#include <cstdio>

int LInfDistance(CvScalar p1, CvScalar p2) {
  int maxVal = 0;
  for (int i = 0; i < 3; i++) {
    int val = abs(p1.val[i] - p2.val[i]);
    if (val > maxVal) {
      maxVal = val;
    }
  }
  return maxVal;
}


THISCLASS::ComponentAdaptiveBackgroundSubtractionMedian(SwisTrackCore *stc):
  Component(stc, wxT("AdaptiveBackgroundSubtractionMedian")),
  mBackgroundModel(0), mOutputImage(0), 
  mDisplayOutput(wxT("Output"), wxT("After background subtraction")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryPreprocessingColor);
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
	IplImage *inputImage = mCore->mDataStructureImageColor.mImage;
	if (! inputImage) {
		AddError(wxT("No input image."));
		return;
	}
	if (inputImage->nChannels != 3) {
		AddError(wxT("The input image is not a color image."));
		return;
	}
	if (!mBackgroundModel) {
  	cvSaveImage("aaaaa.jpg", inputImage);

	  mBackgroundModel = cvCloneImage(inputImage);
	}
	char filename[30];
	sprintf(filename, "Baseline%d.jpg", mCore->mDataStructureInput.mFrameNumber);
	cvSaveImage(filename, inputImage);

	if (! mOutputImage) {
	  mOutputImage = cvCloneImage(inputImage);
	}

	UpdateBackgroundModel(inputImage);
	
	mCore->mDataStructureImageColor.mImage = mOutputImage;
	// Set the display
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) {
	  de.SetMainImage(mOutputImage);
	  //de.SetMainImage(mBackgroundModel);
	}
}

void THISCLASS::UpdateBackgroundModel(IplImage * inputImage) 
{
  
  for (int y = 0; y < mBackgroundModel->height; y++) {
    uchar * bgptr = (uchar *) (mBackgroundModel->imageData + y * mBackgroundModel->widthStep);
    uchar * cptr = (uchar *) (inputImage->imageData + y * mBackgroundModel->widthStep);
    for (int x = 0; x < mBackgroundModel->width; x++) {
      for (int i = 0; i < 3; i++) {
	int bgval = bgptr[3*x + i];
	int imgval = cptr[3*x + i];
	if (bgval < imgval) {
	  bgptr[3*x + i] += 1;
	} else if (bgval > imgval) {
	  bgptr[3*x + i] -= 1;
	} else {
	  // nothing if they are equal
	}
      }
    }
  }
  cvAbsDiff(inputImage, mBackgroundModel, mOutputImage);
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {

}
