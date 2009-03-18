#include "ComponentColorHistogramParticles.h"
#define THISCLASS ComponentColorHistogramParticles

#include "DisplayEditor.h"
#include "Utility.h"
#include <limits>
#include <iostream>
#include <set>
#include <Blob.h>
#include <BlobResult.h>

using namespace std;

THISCLASS::ComponentColorHistogramParticles(SwisTrackCore *stc):
		Component(stc, wxT("ColorHistogramParticles")),
		mParticleMask(NULL), mOutputImage(NULL),
		mMinArea(0),
		mDisplayOutput(wxT("Output"), wxT("Tracking"))

{

	// Data structure relations
	mCategory = &(mCore->mCategoryTracking);
	AddDataStructureRead(&(mCore->mDataStructureImageColor));
	AddDataStructureRead(&(mCore->mDataStructureImageBinary));
	AddDataStructureWrite(&(mCore->mDataStructureParticles));
	AddDisplay(&mDisplayOutput);

	Initialize();						// Read the XML configuration file
	memset(mInputChannels, 0, 3);
	rng = cvRNG(4234234234);
}

THISCLASS::~ComponentColorHistogramParticles()
{

}

void THISCLASS::OnStart()
{
	THISCLASS::OnReloadConfiguration();

}

void THISCLASS::OnReloadConfiguration()
{
  mMinArea = GetConfigurationInt(wxT("MinArea"), 50);
}	

void THISCLASS::OnStep()
{


	wxTimeSpan timeSinceLastFrame = mCore->mDataStructureInput.TimeSinceLastFrame();
	IplImage *foregroundMask = mCore->mDataStructureImageBinary.mImage;
	if (! foregroundMask) {
		AddError(wxT("No foreground mask."));
		return;
	}

	IplImage *inputImage = mCore->mDataStructureInput.mImage;
	if (! inputImage) {
		AddError(wxT("No input image."));
		return;
	}


	if (mInputChannels[0] == NULL) {
	  for (int i = 0; i < 3; i++) {
	    mInputChannels[i] = cvCreateImage(cvGetSize(inputImage), inputImage->depth, 1);
	  }
	}	  
	if (mParticleMask == NULL) {
	  mParticleMask = cvCreateImage(cvGetSize(inputImage), inputImage->depth, 1);
	}

	if (mOutputImage == NULL) {
	  mOutputImage = cvCloneImage(inputImage);

	}
	
	CBlobResult blobs;
	blobs = CBlobResult(foregroundMask, NULL, 0, true);
	blobs.Filter(blobs, B_EXCLUDE, CBlobGetMean(), B_EQUAL, 0);

	cvZero(mOutputImage);
	DataStructureParticles::tParticleVector * particles = 
	  mCore->mDataStructureParticles.mParticles;
	for (DataStructureParticles::tParticleVector::iterator pIt = 
	       particles->begin(); pIt != particles->end(); pIt++) {
	  CBlobGetXYInside op(cvPoint(pIt->mCenter.x, pIt->mCenter.y));


	  bool found = false;

	  
	  CBlob blob;	  
	  for (int i = 0; i < blobs.GetNumBlobs(); i++) {
	    blob = blobs.GetBlob(i);
	    if (blob.Area() > mMinArea && op(blob) == 1){
	      found = true;
	      break;
	    }
	  }
	  if (found) {
	    cvZero(mParticleMask);
	    blob.FillBlob(mParticleMask, cvScalar(255));
	    blob.FillBlob(mOutputImage, CV_RGB(cvRandInt(&rng), cvRandInt(&rng), cvRandInt(&rng)));
	    int histSizes[] = {100,100,100};
	    CvHistogram * histogram = cvCreateHist(3, histSizes, CV_HIST_ARRAY);
	    cvSplit(inputImage, mInputChannels[0], mInputChannels[1], mInputChannels[2], NULL);
	    cvCalcHist(mInputChannels, histogram);
	    pIt->mColorModel = histogram;
	  }
	}


	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) {
	  de.SetMainImage(mOutputImage);

	}
}

void THISCLASS::OnStepCleanup() {
	mCore->mDataStructureParticles.mParticles = 0;
}

void THISCLASS::OnStop() {
}

