#include "ComponentBlobDetectionCircularHough.h"
#define THISCLASS ComponentBlobDetectionCircularHough

#include "DisplayEditor.h"
#define PI 3.14159265358979
#include <algorithm>



THISCLASS::ComponentBlobDetectionCircularHough(SwisTrackCore *stc):
		Component(stc, wxT("BlobDetectionCircularHough")),
		mParticles(),
		mDisplayOutput(wxT("Output"), wxT("Circular particles")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryParticleDetection);
	AddDataStructureRead(&(mCore->mDataStructureImageGray));
	AddDataStructureWrite(&(mCore->mDataStructureParticles));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentBlobDetectionCircularHough() {
}

void THISCLASS::OnStart() 
{
	OnReloadConfiguration();
	return;
}

void THISCLASS::OnReloadConfiguration() 
{
	// Acumulator resolution for Circular Hough Transform	
	dp = GetConfigurationInt(wxT("dp"), 1);
	// Minimum distance between circles
	MinDist = GetConfigurationInt(wxT("MinDist"), 50.);
	// Canny Edge Detection threshold
	Param1 = GetConfigurationInt(wxT("Param1"), 200);
	// Circle Center Detection threshold
	Param2 = GetConfigurationInt(wxT("Param2"), 10);
	// Thresholding circle radius
	SelectionBySize = GetConfigurationBool(wxT("SelectionBySize"), false);
	// Minimum circle radius
	MinRadius = GetConfigurationInt(wxT("MinRadius"), 10);
	// Maximum circle radius
	MaxRadius = GetConfigurationInt(wxT("MaxRadius"), 50);
	// Limit number of circles
	LimitNumberofCircles = GetConfigurationBool(wxT("LimitNumberofCircles"), false);
	// Maximum number of circles
	MaxNumberofCircles = GetConfigurationInt(wxT("MaxNumberofCircles"), 20);

	// Check for errors in configurations
	//if (dp != 1 || dp != 2)
	//	AddError(wxT("The acumulator resolution must be 1 or 2."));
	if (MinDist <= 0)
		MinDist = 1;
	if (Param1 <= 0)
		Param1 = 1;
	if (Param2 <= 0)
		Param2 = 1;
	if (MinRadius <= 0)
		MinRadius = 1;
	if (MaxRadius <= MinRadius)
		MaxRadius = MinRadius + 1;
	if (MaxNumberofCircles <= 0)
		MaxNumberofCircles = 1;
}

void THISCLASS::OnStep() 
{
	// Get and check input image
	IplImage *inputimage = mCore->mDataStructureImageGray.mImage;
	IplImage *outputimage = cvCreateImage(cvSize( inputimage->width,
						      inputimage->height), 8, 3 );
	cvMerge(inputimage,inputimage,inputimage, NULL, outputimage);
	if (! inputimage) {
		AddError(wxT("No input image."));
		return;
	}
	if (! outputimage) {
		AddError(wxT("No output image."));
		return;
	}

	if (inputimage->nChannels != 1) {
		AddError(wxT("The input image is not a gray image."));
		return;
	}
	if (!SelectionBySize)
	{
		MinRadius = 1;
		MaxRadius = 1000;
	}
	CvMemStorage* storage = cvCreateMemStorage(0);
    	CvSeq* circles = cvHoughCircles(inputimage, storage, 
        CV_HOUGH_GRADIENT, dp, MinDist, Param1, Param2,MinRadius,MaxRadius);
	mCore->mEventRecorder->Add(SwisTrackCoreEventRecorder::sType_StepLapTime, this);


	// We clear the output vector
	mParticles.clear();

    	int i;
	int j = 1;    	
	for (i = 0; i < circles->total; i++) 
    	{
        	float* p = (float*)cvGetSeqElem( circles, i );
		// cvCircle - draws circle(image, center, radius, color, thickness)        	 
		// x=cvRound(p[0])
		// y=cvRound(p[1])
		// radius=cvRound(p[2])
	  cvCircle( outputimage, cvPoint(cvRound(p[0]), cvRound(p[1])), cvRound(p[2]), CV_RGB(255,0,0), 2 );
		Particle newparticle;
		newparticle.mCenter.x = p[0];
		newparticle.mCenter.y = p[1];
		newparticle.mArea = 3.14159265*p[2]*p[2];
		if (LimitNumberofCircles)
		{
			if (j > MaxNumberofCircles)
				break;		
		}
		j++;
	}

	// Set these particles
	mCore->mDataStructureParticles.mParticles = &mParticles;
	  //mCore->mDataStructureImageColor.mImage = outputimage;
	// Let the DisplayImage know about our image
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) {
		de.SetParticles(&mParticles);
		de.SetMainImage(outputimage);
	}
}

void THISCLASS::OnStepCleanup() {
	mCore->mDataStructureParticles.mParticles = 0;
}

void THISCLASS::OnStop() {
}

