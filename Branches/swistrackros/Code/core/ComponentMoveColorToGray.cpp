#include "ComponentMoveColorToGray.h"
#define THISCLASS ComponentMoveColorToGray

#include "DisplayEditor.h"

THISCLASS::ComponentMoveColorToGray(SwisTrackCore *stc):
		Component(stc, wxT("MoveColorToGray")),
		mOutputImage(0),
		mDisplayOutput(wxT("Output"), wxT("Move the color image to the gray.")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryPreprocessingColor);
	AddDataStructureRead(&(mCore->mDataStructureImageColor));
	AddDataStructureWrite(&(mCore->mDataStructureImageGray));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentMoveColorToGray() {
}

void THISCLASS::OnStart()
{
	OnReloadConfiguration();
}

void THISCLASS::OnReloadConfiguration()
{

}

void THISCLASS::OnStep()
{
	IplImage *inputimage = mCore->mDataStructureImageColor.mImage;
	if (! inputimage) {
		return;
	}
	if (!mOutputImage)
		mOutputImage = cvCreateImage(cvSize(inputimage->width, inputimage->height), inputimage->depth, 1);
	if (inputimage->nChannels != 3) {
		AddError(wxT("The input image is not a color image."));
		return;
	}

	cvCvtColor(inputimage, mOutputImage, CV_RGB2GRAY);

	mCore->mDataStructureImageGray.mImage = mOutputImage;

	// Let the Display know about our image
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) {
		de.SetMainImage(mCore->mDataStructureImageGray.mImage);
	}
}

void THISCLASS::OnStepCleanup() {

}

void THISCLASS::OnStop() {
	if (mOutputImage) {
		cvReleaseImage(&mOutputImage);
	}
}
