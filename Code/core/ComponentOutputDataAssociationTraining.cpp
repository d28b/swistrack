#include "ComponentOutputDataAssociationTraining.h"
#define THISCLASS ComponentOutputDataAssociationTraining

#include "DisplayEditor.h"
#include "DataAssociationClassifier.h"
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
      DataAssociationClassifier::FeatureVector features = 
	DataAssociationClassifier::ComputeFeatureVector(*p1, *p2);
      if (! mFileStream.is_open()) {
	mFileStream.open(mFileName.ToAscii(), fstream::out | fstream::trunc);
	mFileStream << std::setprecision(15);
	for (DataAssociationClassifier::FeatureVector::iterator i = features.begin();
	     i != features.end(); i++) {      
	  mFileStream << i->first << "\t";
	  cout << "header: " << i->first << endl;
	}
	
	mFileStream << endl;
      }


      for (DataAssociationClassifier::FeatureVector::iterator i = features.begin();
	     i != features.end(); i++) {      
	mFileStream << i->second << "\t";
      }
      mFileStream << endl;
    }
  }
}

void THISCLASS::FilterParticles() {
  for (DataStructureParticles::tParticleVector::iterator pIt = mParticles.begin(); 
       pIt != mParticles.end(); pIt++) {
    wxTimeSpan diff = mCore->mDataStructureInput.FrameTimestamp().Subtract(pIt->mTimestamp);
    if (diff.IsLongerThan(mWindowSize)) {
      cvReleaseHist(&pIt->mColorModel);
      mParticles.erase(pIt);
    }
  }
}

void THISCLASS::BufferParticles(const DataStructureParticles::tParticleVector * inputParticles) {
  for (DataStructureParticles::tParticleVector::const_iterator pIt = inputParticles->begin(); 
       pIt != inputParticles->end(); pIt++) {
    mParticles.push_back(*pIt);
    if (pIt->mColorModel != NULL) {
      cvCopyHist(pIt->mColorModel,  &mParticles.back().mColorModel);
    }
  }

  mFileStream.flush();
}

void THISCLASS::OnReloadConfiguration() {
  mFileName = GetConfigurationString(wxT("FileName"), wxT(""));
  mFileStream.close();
  

  mBufferedFrameCount = GetConfigurationInt(wxT("BufferedFrameCount"), 10);
  mWindowSize = wxTimeSpan::Seconds(GetConfigurationInt(wxT("WindowSizeSeconds"), 2));
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
  mFileStream.close();
  cvReleaseImage(&mOutputImage);
}
