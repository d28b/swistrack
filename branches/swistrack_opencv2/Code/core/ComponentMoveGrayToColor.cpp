#include "ComponentMoveGrayToColor.h"
#define THISCLASS ComponentMoveGrayToColor

#include "DisplayEditor.h"

THISCLASS::ComponentMoveGrayToColor(SwisTrackCore *stc):
		Component(stc, wxT("MoveGrayToColor")),
		mOutputImage(0),
		mDisplayOutput(wxT("Output"), wxT("Move the gray image to the color image.")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryPreprocessingGray);
	AddDataStructureRead(&(mCore->mDataStructureImageGray));
	AddDataStructureWrite(&(mCore->mDataStructureImageColor));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentMoveGrayToColor() {
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
	IplImage *inputimage = mCore->mDataStructureImageGray.mImage;
	if (! inputimage) {
		return;
	}
	if (!mOutputImage)
		mOutputImage = cvCreateImage(cvSize(inputimage->width, inputimage->height), inputimage->depth, 3);
	if (inputimage->nChannels != 1)
	{
		AddError(wxT("This function require a Gray input Image"));
	}

	cvCvtColor(inputimage, mOutputImage, CV_GRAY2RGB);

	mCore->mDataStructureImageColor.mImage = mOutputImage;

	// Let the Display know about our image
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) {
		de.SetMainImage(mCore->mDataStructureImageColor.mImage);
	}
}

void THISCLASS::OnStepCleanup() {

}

void THISCLASS::OnStop() {
	if (mOutputImage) {
		cvReleaseImage(&mOutputImage);
	}
}
