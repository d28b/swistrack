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
		mParticleMask(NULL),
		mDisplayOutput(wxT("Output"), wxT("Tracking"))

{
	// Data structure relations
	mCategory = &(mCore->mCategoryTracking);
	AddDataStructureRead(&(mCore->mDataStructureImageColor));
	AddDataStructureRead(&(mCore->mDataStructureImageBinary));
	AddDataStructureWrite(&(mCore->mDataStructureParticles));
	AddDisplay(&mDisplayOutput);

	Initialize();						// Read the XML configuration file
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
	
	CBlobResult blobs;
	blobs = CBlobResult(foregroundMask, NULL, 0, true);
	


	DataStructureParticles::tParticleVector * particles = 
	  mCore->mDataStructureParticles.mParticles;
	for (DataStructureParticles::tParticleVector::iterator pIt = 
	       particles->begin(); pIt != particles->end(); pIt++) {
	  CBlobGetXYInside op(cvPoint(pIt->mCenter.x, pIt->mCenter.y));

	  CBlob * blob = NULL;
	  for (int i = 0; i < blobs.GetNumBlobs(); i++) {
	    if (op(blob) == 1) {
	      blob = blobs.GetBlob(i);
	    }
	  }
	  if (blob != NULL) {
	    cvZero(mParticleMask);
	    blob->FillBlob(mParticleMask, cvScalar(255));
	    int histSizes[] = {100,100,100};
	    CvHistogram * histogram = cvCreateHist(3, histSizes, CV_HIST_ARRAY);
	    cvSplit(inputImage, mInputChannels[0], mInputChannels[1], mInputChannels[2], NULL);
	    cvCalcHist(mInputChannels, histogram);
	    pIt->mColorModel = histogram;
	  }
	}


	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) {
	  de.SetMainImage(mParticleMask);

	}
}

void THISCLASS::OnStepCleanup() {
	mCore->mDataStructureParticles.mParticles = 0;
}

void THISCLASS::OnStop() {
}

