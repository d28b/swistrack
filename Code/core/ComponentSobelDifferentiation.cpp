#include "ComponentSobelDifferentiation.h"
#define THISCLASS ComponentSobelDifferentiation

#include "DisplayEditor.h"
#define PI 3.14159265358979
#include <algorithm>

THISCLASS::ComponentSobelDifferentiation(SwisTrackCore *stc):
		Component(stc, wxT("SobelDifferentiation")),
		mDisplayOutput(wxT("Output"), wxT("Differentiation")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryPreprocessingGray);
	AddDataStructureRead(&(mCore->mDataStructureImageGray));
	AddDataStructureWrite(&(mCore->mDataStructureImageGray));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentSobelDifferentiation() {}

void THISCLASS::OnStart() 
{
	OnReloadConfiguration();
	return;
}

void THISCLASS::OnReloadConfiguration() 
{
		
	// Differentiation Order for x
	xorder = GetConfigurationInt(wxT("xorder"), 1);
	// Differentiation Order for y
	yorder = GetConfigurationInt(wxT("yorder"), 1);
	// Size of the extended Sobel kernel
	apertureSize = GetConfigurationInt(wxT("apertureSize"), 3);
	
	// Check for errors in configurations
	if (xorder < 0 )
		xorder = 0;
	if (xorder > 3 )
		xorder = 3;	
	if (yorder < 0 )
		yorder = 0;
	if (yorder > 3 )
		yorder = 3;
	// Ensuring the differentiation order is less than the kernel size 
	if (apertureSize == 1 || apertureSize == 3)
	{
		if (xorder > 2)
			xorder = 2;
		if (yorder > 2)
			yorder = 2;
	}
	// Ensuring that at least 1 differentiation will be made
	if (xorder == 0 && yorder == 0)
		xorder = 1; 
}

void THISCLASS::OnStep() 
{
		
	// Get and check input image
	IplImage *inputimage = mCore->mDataStructureImageGray.mImage;
	IplImage *outputimage = mCore->mDataStructureImageGray.mImage;	
	if (! inputimage) {
		AddError(wxT("No input image."));
		return;
	}
	if (inputimage->nChannels != 1) {
		AddError(wxT("The input image is not a gray image."));
		return;
	}
    	cvSobel( inputimage, outputimage, xorder, yorder, apertureSize );
	// I : Source image. 
	// J : Destination image. 
	// dx : Order of the derivative x . 
	// dy : Order of the derivative y . 
	// apertureSize : Size of the extended Sobel kernel, must be 1, 3, 5 or 7
	mCore->mEventRecorder->Add(SwisTrackCoreEventRecorder::sType_StepLapTime, this);
	
	mCore->mDataStructureImageGray.mImage = outputimage;
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

