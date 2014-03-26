#include "ComponentMorphology.h"
#define THISCLASS ComponentMorphology

#include "DisplayEditor.h"
#define PI 3.14159265358979
#include <algorithm>

THISCLASS::ComponentMorphology(SwisTrackCore *stc):
		Component(stc, wxT("Morphology")),
		mDisplayOutput(wxT("Output"), wxT("Morphology")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryPreprocessingBinary);
	AddDataStructureRead(&(mCore->mDataStructureImageBinary));
	AddDataStructureWrite(&(mCore->mDataStructureImageBinary));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentMorphology() {}

void THISCLASS::OnStart() 
{
	OnReloadConfiguration();
	return;
}

void THISCLASS::OnReloadConfiguration() 
{
		
	// Type of operation for morphology
	operation = GetConfigurationInt(wxT("operation"), 1);
	// Number of iterations for morphology
	iterations = GetConfigurationInt(wxT("iterations"), 1);
	
	// Check for errors in configurations	
	if (iterations < 0 )
		iterations = 0;
	// Convert operation number into operation
}

void THISCLASS::OnStep() 
{
	 	
	// Get and check input image
	IplImage *inputimage = mCore->mDataStructureImageBinary.mImage;	
	if (! inputimage) {
		AddError(wxT("No input image."));
		return;
	}
	if (inputimage->nChannels != 1) {
		AddError(wxT("The input image is not a binary image."));
		return;
	}
	

	/*
	Five different operations:	
	Opening:
	dst = dilate(erode(src,element),element)
	
	Closing:
	dst = erode(dilate(src,element),element)
	
	Morphological gradient:
	dst = dilate(src,element)-erode(src,element)
	
	"Top hat":
	dst = src-open(src,element)
	
	"Black hat":
	dst = close(src,element)-src
	*/
	tempimage = cvCloneImage( inputimage );
	tempimage2 = cvCloneImage( inputimage );
	outputimage = cvCloneImage( inputimage );

	if (operation == CV_MOP_OPEN)
	{
		cvErode( inputimage, tempimage, NULL, iterations);		
		cvDilate( tempimage, outputimage, NULL, iterations);
	}
	else if (operation == CV_MOP_CLOSE)
	{
		cvDilate( inputimage, tempimage, NULL, iterations);		
		cvErode( tempimage, outputimage, NULL, iterations);		
	}
	else if (operation == CV_MOP_GRADIENT)
	{
		cvDilate( inputimage, tempimage, NULL, iterations);		
		cvErode( inputimage, tempimage2, NULL, iterations);
		cvSub(tempimage, tempimage2, outputimage, NULL);				
	}
	else if (operation == CV_MOP_TOPHAT)
	{
		cvErode( inputimage, tempimage, NULL, iterations);		
		cvDilate( tempimage, tempimage2, NULL, iterations);
		cvSub(inputimage, tempimage2, outputimage, NULL);
	}
	else if (operation == CV_MOP_BLACKHAT)
	{
		cvDilate( inputimage, tempimage, NULL, iterations);		
		cvErode( tempimage, tempimage2, NULL, iterations);
		cvSub(tempimage2, inputimage, outputimage, NULL);
	}
	
	IplImage *OutputImage = outputimage;
	mCore->mEventRecorder->Add(SwisTrackCoreEventRecorder::sType_StepLapTime, this);
	mCore->mDataStructureImageBinary.mImage = OutputImage;
	// Let the DisplayImage know about our image
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) {
		de.SetMainImage(OutputImage);
	}
}

void THISCLASS::OnStepCleanup() {
	
}

void THISCLASS::OnStop() {
}

