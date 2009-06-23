#include "ComponentTomatoDetector.h"
#define THISCLASS ComponentTomatoDetector

#include "DisplayEditor.h"
#define PI 3.14159265358979
#include <algorithm>
#include "Utility.h"
#include <limits>
#include <iostream>
#include <set>

//////////////////////////////////////////////////////////////////////////////////////////
// Component that integrates 4 different methods to detect tomatoes. Before this component
// there should be an input from USB camera and a conversion to color.
//////////////////////////////////////////////////////////////////////////////////////////





THISCLASS::ComponentTomatoDetector(SwisTrackCore *stc):
		Component(stc, wxT("TomatoDetector")),
		mDisplayOutput(wxT("Output"), wxT("Tomatoes")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryParticleDetection);
	AddDataStructureWrite(&(mCore->mDataStructureParticles));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentTomatoDetector() {}

void THISCLASS::OnStart() 
{
	OnReloadConfiguration();
	return;
}

void THISCLASS::OnReloadConfiguration() 
{
	// Select image to display
	selected_image = GetConfigurationInt(wxT("selected_image"), 1);
	GrayThreshold = GetConfigurationInt(wxT("GrayThreshold"), 226);
	ErosionIterations = GetConfigurationInt(wxT("ErosionIterations"), 1);
	MaxNumberBlob = GetConfigurationInt(wxT("MaxNumberBlob"), 10);
	MaxAreaBlob = GetConfigurationInt(wxT("MaxAreaBlob"), 100);
	MinAreaBlob = GetConfigurationInt(wxT("MinAreaBlob"), 500);	
	MinCompactnessBlob = GetConfigurationDouble(wxT("MinCompactnessBlob"), 0.4);
	MaxCompactnessBlob = GetConfigurationDouble(wxT("MaxCompactnessBlob"), 1);
	MinParticleArea = GetConfigurationDouble(wxT("MinParticleArea"), 100);
	MaxParticleArea = GetConfigurationDouble(wxT("MaxParticleArea"), 100000.0);
	
}

void THISCLASS::OnStep() 
{	
	// Get and check input image
	inputimage = mCore->mDataStructureImageColor.mImage;
	outputimage = mCore->mDataStructureImageColor.mImage;	
	tempimagebinary = cvCreateImage(cvSize(inputimage->width, inputimage->height), inputimage->depth, 1);
	if (! inputimage) {
		AddError(wxT("No input image."));
		return;
	}
	if (inputimage->nChannels != 3) {
		AddError(wxT("The input image is not a color image."));
		return;
	}
	if (selected_image == 1)
	{
		DisplayEditor de(&mDisplayOutput);
		if (de.IsActive()) 
		{
			de.SetMainImage(inputimage);
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////
	//***************************************************************************** //	
	// Light Reflexion Tomato Detector Component
	//***************************************************************************** //	
	/////////////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////////////////
	// Color to Gray conversion
	/////////////////////////////////////////////////////////////////////////////////	
	cvCvtColor(inputimage, tempimagebinary, CV_RGB2GRAY);						// converts the color image to gray
	mCore->mDataStructureImageGray.mImage = tempimagebinary;
	if (selected_image == 2)
	{
		DisplayEditor de(&mDisplayOutput);
		if (de.IsActive()) 
		{
			de.SetMainImage(tempimagebinary);
		}
	}

	/////////////////////////////////////////////////////////////////////////////////
	// Gray Threshold
	////////////////////////////////////////////////////////////////////////////////	
	// default value for Threshold = 221;
	cvThreshold(tempimagebinary, tempimagebinary, GrayThreshold, 255, CV_THRESH_BINARY);		
	mCore->mDataStructureImageBinary.mImage = tempimagebinary;
	
	/////////////////////////////////////////////////////////////////////////////////
	// Erosion
	/////////////////////////////////////////////////////////////////////////////////											// 
	cvErode(tempimagebinary, tempimagebinary, NULL, ErosionIterations);				
	mCore->mDataStructureImageBinary.mImage = tempimagebinary;
	if (selected_image == 3)
	{
		DisplayEditor de(&mDisplayOutput);
		if (de.IsActive()) 
		{
			de.SetMainImage(tempimagebinary);
		}
	}
	/////////////////////////////////////////////////////////////////////////////////	
	// Blob Detection
	////////////////////////////////////////////////////////////////////////////////
	std::vector<Particle> rejectedparticles;
	// Get and check input image
	tempimagebinary2 = cvCloneImage(mCore->mDataStructureImageBinary.mImage);
	//mCore->mDataStructureImageBinary.mImage;
	if (! tempimagebinary2) 
	{
		AddError(wxT("No input image."));
		return;
	}
	if (tempimagebinary2->nChannels != 1) 
	{
		AddError(wxT("The input image is not a binary image."));
		return;
	}

	// We clear the ouput vector
	mParticles.clear();

	// Initialization
	Particle tmpParticle; // Used to put the calculated value in memory
	CvMoments moments; // Used to calculate the moments
	std::vector<Particle>::iterator j; // Iterator used to stock the particles by size

	// We allocate memory to extract the contours from the binary image
	CvMemStorage* storage = cvCreateMemStorage(0);
	CvSeq* contour = 0;


	// Init blob extraxtion
	CvContourScanner blobs = cvStartFindContours(tempimagebinary2, storage, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_NONE);

	// This is used to correct the position in case of ROI
	CvRect rectROI;
	if (tempimagebinary2->roi != NULL) 
	{
		rectROI = cvGetImageROI(tempimagebinary2);
	} 
	else 
	{
		rectROI.x = 0;
		rectROI.y = 0;
	}

	while ((contour = cvFindNextContour(blobs)) != NULL) 
	{
		// Computing the moments
		cvMoments(contour, &moments);

		// Computing particle area
		tmpParticle.mArea = moments.m00;
		tmpParticle.mCenter.x = (float)(rectROI.x + (moments.m10 / moments.m00 + 0.5));  // moments using Green theorem
		tmpParticle.mCenter.y = (float)(rectROI.y + (moments.m01 / moments.m00 + 0.5));  // m10 = x direction, m01 = y direction, m00 = area as edicted in theorem
		tmpParticle.mTimestamp = mCore->mDataStructureInput.FrameTimestamp();
		tmpParticle.mFrameNumber = mCore->mDataStructureInput.mFrameNumber;

		// Selection based on area
		if (((tmpParticle.mArea <= MaxAreaBlob) && (tmpParticle.mArea >= MinAreaBlob)))
		{
			tmpParticle.mCompactness = GetContourCompactness(contour);
			if ((tmpParticle.mCompactness > MinCompactnessBlob) && (tmpParticle.mCompactness < MaxCompactnessBlob))
			{
			
				// Check if we have already enough particles
				if (mParticles.size() == MaxNumberBlob)
				{
					// If the particle is bigger than the smallest stored particle, store it, else do nothing
					if (tmpParticle.mArea > mParticles.back().mArea)
					{
						// Find the place were it must be inserted, sorted by size
						for (j = mParticles.begin(); (j != mParticles.end()) && (tmpParticle.mArea < (*j).mArea); j++);

						// Fill unused values
						tmpParticle.mID = -1;
						tmpParticle.mIDCovariance = -1;

						// Insert the particle
						mParticles.insert(j, tmpParticle);
						// Remove the smallest one
						mParticles.pop_back();
					}
				}
				else
				{
					// The particle is added at the correct place
					// Find the place were it must be inserted, sorted by size
					for (j = mParticles.begin(); (j != mParticles.end()) && (tmpParticle.mArea < (*j).mArea); j++);

					// Fill unused values
					tmpParticle.mID = -1;
					tmpParticle.mIDCovariance = -1;

					// Insert the particle
					mParticles.insert(j, tmpParticle);
				}
			}
		}
		else
		{
			rejectedparticles.push_back(tmpParticle);
		}
		cvRelease((void**)&contour);
	}
	contour = cvEndFindContours(&blobs);
	cvReleaseImage(&tempimagebinary2);
	cvRelease((void**)&contour);
	cvReleaseMemStorage(&storage);

	// Set these particles
	mCore->mDataStructureParticles.mParticles = &mParticles;
	if (selected_image == 4)
	{
		DisplayEditor de(&mDisplayOutput);
		if (de.IsActive()) 
		{
			de.SetParticles(&mParticles);
			de.SetMainImage(inputimage);
		}
	}
	

	/////////////////////////////////////////////////////////////////////////////////
	// Filter Particles
	/////////////////////////////////////////////////////////////////////////////////
  	particle_image = mCore->mDataStructureInput.mImage;
  	outputimage = particle_image;
  
  	cvCopy(mCore->mDataStructureInput.mImage, outputimage);
  	FinalParticles.clear();
  	//DataStructureParticles::tParticleVector * particles = mCore->mDataStructureParticles.mParticles;
  	for (DataStructureParticles::tParticleVector::iterator pIt = mParticles.begin(); pIt != mParticles.end(); pIt++) 
	{	
    		const Particle & p = *pIt;
    		//cout << "Area: " << p.mArea << endl;
    		//cout << "Min: " << mMinParticleArea << endl;
    		//cout << "Max: " << mMaxParticleArea << endl;
    		if (p.mArea >= MinParticleArea && p.mArea <= MaxParticleArea) 
		{
      			//cout << "Adding particle " << endl;
      			FinalParticles.push_back(p);
      			cvCircle(outputimage, cvPointFrom32f(p.mCenter), 10, CV_RGB(0, 255, 0), 3, CV_AA, 0 );
    		}
    		cvCircle(outputimage, cvPointFrom32f(p.mCenter), 10, CV_RGB(255, 0, 0), 3, CV_AA, 0 );
  	}

  	mCore->mDataStructureParticles.mParticles = &FinalParticles;
	if (selected_image == 5)
	{  	
		DisplayEditor de(&mDisplayOutput);
  		if (de.IsActive()) 
		{
    			//de.SetParticles(&mParticles);
    			de.SetMainImage(outputimage);
  		}
	}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	




}


void THISCLASS::OnStepCleanup() {
	mCore->mDataStructureParticles.mParticles = 0;
	cvReleaseImage(&tempimagebinary);
	}

void THISCLASS::OnStop() {
}

double THISCLASS::GetContourCompactness(const void* contour) 
{
	double l = cvArcLength(contour, CV_WHOLE_SEQ, 1);
	return fabs(12.56*cvContourArea(contour) / (l*l));
}
