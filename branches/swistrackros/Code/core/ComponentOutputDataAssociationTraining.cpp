#include "ComponentOutputDataAssociationTraining.h"
#define THISCLASS ComponentOutputDataAssociationTraining

#include "DisplayEditor.h"
#include "DataAssociationClassifier.h"
#include "Utility.h"
#include <iomanip>
#include <iostream>
using namespace std;


THISCLASS::ComponentOutputDataAssociationTraining(SwisTrackCore *stc):
  Component(stc, wxT("OutputDataAssociationTraining")),
  mOutputImage(0),
  mDisplayOutput(wxT("Output"), wxT("Tracking"))
{

	// Data structure relations
	mCategory = &(mCore->mCategoryOutput);
	AddDataStructureRead(&(mCore->mDataStructureParticles));
	AddDisplay(&mDisplayOutput);
	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentOutputDataAssociationTraining() {
}

void THISCLASS::OnStart() {
  OnReloadConfiguration();

  mParticles.clear();

}

// Algorithm:
//   1.  new particles come in
//   2.  run on every pair in the buffer, and output
//   3.  delete old ones from the buffer
//   4.  add to the buffer
//
void THISCLASS::OnStep() {


  cout << "Got me " << mParticles.size() << " particles." << endl;
  
  FilterParticles();
  const DataStructureParticles::tParticleVector * particles = mCore->mDataStructureParticles.mParticles;

  OutputTrainingData(particles);

  BufferParticles(particles);



  DisplayEditor de(&mDisplayOutput);
  if (de.IsActive()) {
    de.SetMainImage(mCore->mDataStructureInput.mImage);
    de.SetParticles(&mParticles);
  }
}

void THISCLASS::OutputTrainingData(const DataStructureParticles::tParticleVector * inputParticles) 
{

  for (DataStructureParticles::tParticleVector::const_iterator p1 = mParticles.begin(); 
       p1 != mParticles.end(); p1++) {
    for (DataStructureParticles::tParticleVector::const_iterator p2 = inputParticles->begin(); 
	 p2 != inputParticles->end(); p2++) {
      double distanceSquared = Utility::SquareDistance(p1->mCenter, p2->mCenter);
      if (distanceSquared <= mMaxDistanceSquared) {
	DataAssociationClassifier::Example features = 
	  DataAssociationClassifier::ComputeExample(*p1, *p2);
	if (! mFileStream.is_open()) {
	  mFileStream.open(mFileName.ToAscii(), fstream::out | fstream::trunc);
	  mFileStream << std::setprecision(15);
	  for (DataAssociationClassifier::Example::iterator i = features.begin();
	       i != features.end(); i++) {
	    if (i != features.begin()) {
	      mFileStream << "\t";
	    }
	    mFileStream << i->first;
	  }
	  mFileStream << endl;
	}
	for (DataAssociationClassifier::Example::iterator i = features.begin();
	     i != features.end(); i++) {      
	  if (i != features.begin()) {
	    mFileStream << "\t";
	  }	  
	  mFileStream << i->second;
	}
	mFileStream << endl;
      }
    }
  }
  mFileStream.flush();
}

void THISCLASS::FilterParticles() {
  for (DataStructureParticles::tParticleVector::iterator pIt = mParticles.begin(); 
       pIt != mParticles.end();) {

    wxTimeSpan diff = mCore->mDataStructureInput.FrameTimestamp().Subtract(pIt->mTimestamp);
    if (diff.IsLongerThan(mWindowSize)) {
      cvReleaseHist(&(pIt->mColorModel));
      pIt = mParticles.erase(pIt);
    } else {
      ++pIt;
    }
  }
}

void THISCLASS::BufferParticles(const DataStructureParticles::tParticleVector * inputParticles) {
  for (DataStructureParticles::tParticleVector::const_iterator pIt = inputParticles->begin(); 
       pIt != inputParticles->end(); pIt++) {
    mParticles.push_back(*pIt);
    Particle & p = mParticles.back();
    if (pIt->mColorModel != NULL) {
      p.mColorModel = NULL; // force new memory allocation
      cvCopyHist(pIt->mColorModel,  &p.mColorModel);
    } else {
      p.mColorModel = NULL;
    }

  }
}

void THISCLASS::OnReloadConfiguration() {
  mFileName = GetConfigurationString(wxT("FileName"), wxT(""));
  mFileStream.close();
  


  int millis = GetConfigurationDouble(wxT("WindowSizeMilliseconds"), 2);
  mWindowSize = wxTimeSpan(0, 0, 0, millis);


  mMaxDistanceSquared = GetConfigurationDouble(wxT("MaxDistance"), 200);
  mMaxDistanceSquared *= mMaxDistanceSquared;

}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
  mFileStream.close();
  cvReleaseImage(&mOutputImage);
}
