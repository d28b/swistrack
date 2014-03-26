#include "ComponentMoveColorToBinary.h"
#define THISCLASS ComponentMoveColorToBinary

#include "DisplayEditor.h"

THISCLASS::ComponentMoveColorToBinary(SwisTrackCore *stc):
		Component(stc, wxT("MoveColorToBinary")),
		mOutputImage(0),
		mDisplayOutput(wxT("Output"), wxT("Move the color image to the Binary.")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryPreprocessingColor);
	AddDataStructureRead(&(mCore->mDataStructureImageColor));
	AddDataStructureWrite(&(mCore->mDataStructureImageBinary));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentMoveColorToBinary() {
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

	mCore->mDataStructureImageBinary.mImage = mOutputImage;

	// Let the Display know about our image
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) {
		de.SetMainImage(mCore->mDataStructureImageBinary.mImage);
	}
}

void THISCLASS::OnStepCleanup() {

}

void THISCLASS::OnStop() {
	if (mOutputImage) {
		cvReleaseImage(&mOutputImage);
	}
}
