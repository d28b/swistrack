#include "ComponentBinarySmooth.h"
#define THISCLASS ComponentBinarySmooth

#include "DisplayEditor.h"
#define PI 3.14159265358979
#include <algorithm>

THISCLASS::ComponentBinarySmooth(SwisTrackCore *stc):
		Component(stc, wxT("BinarySmooth")),
		mDisplayOutput(wxT("Output"), wxT("Smooth")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryPreprocessingBinary);
	AddDataStructureRead(&(mCore->mDataStructureImageBinary));
	AddDataStructureWrite(&(mCore->mDataStructureImageBinary));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentBinarySmooth() {}

void THISCLASS::OnStart() 
{
	OnReloadConfiguration();
	return;
}

void THISCLASS::OnReloadConfiguration() 
{
	// Type of smoothing function
	smoothtype = GetConfigurationInt(wxT("smoothtype"), 1);
	// First parameter of smoothing operation
	param = GetConfigurationInt(wxT("param"), 1);
}

void THISCLASS::OnStep() 
{
		
	// Get and check input image
	IplImage *inputimage = mCore->mDataStructureImageBinary.mImage;
	IplImage *outputimage = mCore->mDataStructureImageBinary.mImage;	
	if (! inputimage) {
		AddError(wxT("No input image."));
		return;
	}
	if (inputimage->nChannels != 1) {
		AddError(wxT("The input image is not a binary image."));
		return;
	}
    	cvSmooth(inputimage, outputimage, smoothtype, param, 0, 0, 0);
	mCore->mEventRecorder->Add(SwisTrackCoreEventRecorder::sType_StepLapTime, this);
	mCore->mDataStructureImageBinary.mImage = outputimage;
	// Let the DisplayImage know about our image
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) {
		de.SetMainImage(outputimage);
	}
}

void THISCLASS::OnStepCleanup() {
	
}

void THISCLASS::OnStop() {
}

