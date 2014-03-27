#include "ComponentInvertBinary.h"
#define THISCLASS ComponentInvertBinary

#include "DisplayEditor.h"

THISCLASS::ComponentInvertBinary(SwisTrackCore *stc):
		Component(stc, wxT("InvertBinary")),
		mOutputImage(0),
		mDisplayOutput(wxT("Output"), wxT("Invert binary image.")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryPreprocessingBinary);
	AddDataStructureRead(&(mCore->mDataStructureImageBinary));
	AddDataStructureWrite(&(mCore->mDataStructureImageBinary));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentInvertBinary() {
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
		mOutputImage = cvCreateImage(cvSize(inputimage->width, inputimage->height), inputimage->depth, 1);
	if (inputimage->nChannels != 1)
	{
		AddError(wxT("This function require a Binary input Image"));
	}

	cvXorS(inputimage, cvScalar(255), mOutputImage);


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
