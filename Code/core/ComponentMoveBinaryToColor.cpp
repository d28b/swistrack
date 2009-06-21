#include "ComponentMoveBinaryToColor.h"
#define THISCLASS ComponentMoveBinaryToColor

#include "DisplayEditor.h"

THISCLASS::ComponentMoveBinaryToColor(SwisTrackCore *stc):
		Component(stc, wxT("MoveBinaryToColor")),
		mOutputImage(0),
		mDisplayOutput(wxT("Output"), wxT("Move the binary image to the color image.")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryPreprocessingBinary);
	AddDataStructureRead(&(mCore->mDataStructureImageBinary));
	AddDataStructureWrite(&(mCore->mDataStructureImageColor));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentMoveBinaryToColor() {
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
	IplImage *inputimage = mCore->mDataStructureImageBinary.mImage;
	if (! inputimage) {
		return;
	}
	if (!mOutputImage)
		mOutputImage = cvCreateImage(cvSize(inputimage->width, inputimage->height), inputimage->depth, 3);
	if (inputimage->nChannels != 1)
	{
		AddError(wxT("This function require a Gray input Image"));
	}

	cvCvtColor(inputimage, mOutputImage, CV_GRAY2BGR);

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
