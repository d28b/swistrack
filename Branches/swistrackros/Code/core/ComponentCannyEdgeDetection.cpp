#include "ComponentCannyEdgeDetection.h"
#define THISCLASS ComponentCannyEdgeDetection

#include "DisplayEditor.h"

THISCLASS::ComponentCannyEdgeDetection(SwisTrackCore *stc):
		Component(stc, wxT("CannyEdgeDetection")),
		mOutputImage(0), 
		mDisplayOutput(wxT("Output"), wxT("After thresholding")) {

	// Data structure relations

	mCategory = &(mCore->mCategoryPreprocessingColor);
	AddDataStructureRead(&(mCore->mDataStructureImageGray));
	AddDataStructureWrite(&(mCore->mDataStructureImageBinary));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentCannyEdgeDetection() {
}

void THISCLASS::OnStart()
{
	OnReloadConfiguration();
}

void THISCLASS::OnReloadConfiguration() {

  mThreshold1 = GetConfigurationDouble(wxT("Threshold1"), 50);
  mThreshold2 = GetConfigurationDouble(wxT("Threshold2"), 200);
  
}

void THISCLASS::OnStep() {
	IplImage *inputimage = mCore->mDataStructureImageGray.mImage;
	if (! inputimage)
	{
	  AddError(wxT("Cannot access input image."));
	  return;
	}
	
	if (inputimage->nChannels != 1)
	{
	  AddError(wxT("Input must be a greyscale (1 channel)."));
	}

	if (mOutputImage == NULL) {
	  mOutputImage = cvCreateImage(cvSize(inputimage->width, 
					      inputimage->height), 8, 1);
	}
	

	cvCanny(inputimage, mOutputImage, mThreshold1, mThreshold2);
	mCore->mDataStructureImageBinary.mImage = mOutputImage;
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) {
		de.SetMainImage(mOutputImage);
	}
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
  cvReleaseImage(&mOutputImage);

}
