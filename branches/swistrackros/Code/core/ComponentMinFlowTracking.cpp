#include "ComponentMinFlowTracking.h"
#define THISCLASS ComponentMinFlowTracking

using namespace std;
#include <iostream>
#include <set>

#include "DisplayEditor.h"



class TrajectoryHypothesis {
  vector<DetectionResponse> mParticles;
};

class AssociationHypothesis {
  vector<TrajectoryHypothesis> mHypothesis;
};
  

THISCLASS::ComponentMinFlowTracking(SwisTrackCore *stc):
		Component(stc, wxT("MinFlowTracking")),
		mObservations(),
		mWindowSize(10), mFrameCount(0),
		mDisplayOutput(wxT("Output"), wxT("Dynamic Tracking"))
{
	// Data structure relations
	mCategory = &(mCore->mCategoryTracking);
	AddDataStructureRead(&(mCore->mDataStructureParticles));
	AddDataStructureWrite(&(mCore->mDataStructureParticles));
	AddDataStructureWrite(&(mCore->mDataStructureTracks));
	AddDisplay(&mDisplayOutput);

	Initialize();						// Read the XML configuration file
}

THISCLASS::~ComponentMinFlowTracking()
{
	mTracks.clear();
}

void THISCLASS::OnStart()
{
	THISCLASS::OnReloadConfiguration();
	mFrameCount = 0;
}

void THISCLASS::OnReloadConfiguration()
{
}

void THISCLASS::OnStep()
{

  wxTimeSpan timeSinceLastFrame = mCore->mDataStructureInput.TimeSinceLastFrame();
  if (timeSinceLastFrame.IsLongerThan(wxTimeSpan::Seconds(5))) {

  }
  mFrameCount++;
  
  DataStructureParticles::tParticleVector *particles = 
    mCore->mDataStructureParticles.mParticles;
  
  for (DataStructureParticles::tParticleVector::iterator pIt = 
	 particles->begin(); pIt != particles->end(); pIt++) {
    mObservations.push_back(DetectionResponse(*pIt));
    
  }
  
  if (mFrameCount >= mWindowSize) {
    Track();
    mFrameCount = 0;
    mObservations.clear();
  }
  
  DisplayEditor de(&mDisplayOutput);
  if (de.IsActive()) {
    de.SetMainImage(mCore->mDataStructureInput.mImage);
    de.SetTrajectories(true);
  }
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {

}

void THISCLASS::Track() {
  for (vector<DetectionResponse>::iterator it = mObservations.begin();
       it != mObservations.end(); it++) {
    //DetectionResponse & response = *it;
  }
}
