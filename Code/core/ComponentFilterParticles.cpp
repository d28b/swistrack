#include "ComponentFilterParticles.h"
#define THISCLASS ComponentFilterParticles

#include "DisplayEditor.h"
#include "Utility.h"
#include <limits>
#include <iostream>
#include <set>

using namespace std;

THISCLASS::ComponentFilterParticles(SwisTrackCore *stc):
		Component(stc, wxT("FilterParticles")),
		mParticles(), mOutputImage(0),
		mDisplayOutput(wxT("Output"), wxT("Particles"))

{
	// Data structure relations
	mCategory = &(mCore->mCategoryParticleDetection);
	AddDataStructureRead(&(mCore->mDataStructureParticles));
	AddDataStructureWrite(&(mCore->mDataStructureParticles));
	
	AddDisplay(&mDisplayOutput);
	Initialize();						// Read the XML configuration file
}

THISCLASS::~ComponentFilterParticles()
{

}

void THISCLASS::OnStart()
{
	THISCLASS::OnReloadConfiguration();

}

void THISCLASS::OnReloadConfiguration()
{


  mMaxParticleArea = GetConfigurationDouble(wxT("MaxParticleArea"), 100);
  mMinParticleArea = GetConfigurationDouble(wxT("MinParticleArea"), 100);
  cout << "Value: " << mMinParticleArea <<" " << mMaxParticleArea << endl;

}

void THISCLASS::OnStep()
{
  IplImage *inputImage = mCore->mDataStructureInput.mImage;
  if (mOutputImage == NULL) {
    mOutputImage = cvCloneImage(inputImage);
  }
  cvCopy(mCore->mDataStructureInput.mImage, mOutputImage);
  mParticles.clear();
  DataStructureParticles::tParticleVector * particles = mCore->mDataStructureParticles.mParticles;
  for (DataStructureParticles::tParticleVector::iterator pIt = 
	 particles->begin(); pIt != particles->end(); pIt++) {	
    const Particle & p = *pIt;
    cout << "Area: " << p.mArea << endl;
    cout << "Min: " << mMinParticleArea << endl;
    cout << "Max: " << mMaxParticleArea << endl;
    if (p.mArea >= mMinParticleArea && p.mArea <= mMaxParticleArea) {
      cout << "Adding particle " << endl;
      mParticles.push_back(p);
      cvCircle(mOutputImage, cvPointFrom32f(p.mCenter), 10, CV_RGB(0, 255, 0), 3, CV_AA, 0 );
    }
    cvCircle(mOutputImage, cvPointFrom32f(p.mCenter), 10, CV_RGB(255, 0, 0), 3, CV_AA, 0 );
  }

  mCore->mDataStructureParticles.mParticles = &mParticles;
  DisplayEditor de(&mDisplayOutput);
  if (de.IsActive()) {
    //de.SetParticles(&mParticles);
    de.SetMainImage(mOutputImage);

  }
}

void THISCLASS::OnStepCleanup() {

}

void THISCLASS::OnStop() {
  cvReleaseImage(&mOutputImage);
}



