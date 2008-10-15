#include "ComponentTrackSmoothing.h"
#define THISCLASS ComponentTrackSmoothing

using namespace std;
#include <iostream>

#include "DisplayEditor.h"

THISCLASS::ComponentTrackSmoothing(SwisTrackCore *stc):
		Component(stc, wxT("TrackSmoothing")),
		mDisplayOutput(wxT("Output"), wxT("Smoothing"))
{
	// Data structure relations
	mCategory = &(mCore->mCategoryTracking);
	AddDataStructureRead(&(mCore->mDataStructureTracks));
	AddDataStructureWrite(&(mCore->mDataStructureTracks));
	AddDisplay(&mDisplayOutput);

	Initialize();						// Read the XML configuration file
}

THISCLASS::~ComponentTrackSmoothing()
{
  if (mOutputTracks.size()) {
    mOutputTracks.clear();
  }
  
}
Track& THISCLASS::WindowForTrack(int id) 
{
  DataStructureTracks::tTrackVector::iterator it = mWindows.begin();
  while (it != mWindows.end()) {
    if (it->mID == id) {
      return *it;
    }
    it++;
  }

  mWindows.push_back(Track(id, -1));
  mWindows.back().SetMaxLength(mWindowSize);
  mOutputTracks.push_back(Track(id, -1));
  return mWindows.back();


}
void THISCLASS::OnStart()
{
	mWindowSize = GetConfigurationInt(wxT("WindowSize"), 3);
	mWindows.clear();
	if (mOutputTracks.size()) mOutputTracks.clear();      
}

void THISCLASS::OnStep()
{
  DataStructureTracks::tTrackVector *tracks = 
    mCore->mDataStructureTracks.mTracks;
	if (! tracks)
	{
		AddError(wxT("No Track"));
		return;
	}
	DataStructureParticles::tParticleVector *particles = mCore->mDataStructureParticles.mParticles;
	if (! particles)
	  {
	    AddError(wxT("There are no particles"));
	    return;
	  }
	
	//For each track, write data in the corresponding output file
	DataStructureTracks::tTrackVector::iterator it = tracks->begin();
	while (it != tracks->end())
	{
	  Track & window = WindowForTrack(it->mID);
	  //Search for the corresponding particle
	
	  
	  DataStructureParticles::tParticleVector::iterator it2 = particles->begin();
	  while (it2 != particles->end())
	    {
	      //Correct ID is found
	      if (window.mID == it2->mID) 
		{
		  window.AddPoint(it2->mCenter);

		}
	      it2++;
	    }
	  it++;
	}
	mParticles.clear();
	DataStructureTracks::tTrackVector::iterator outputIterator = mOutputTracks.begin();
	while (outputIterator != mOutputTracks.end()) {
	  Track & window = WindowForTrack(outputIterator->mID);

	  if (window.trajectory.size() == mWindowSize) {
	    CvPoint2D32f point = cvPoint2D32f(0, 0);
	    std::vector<CvPoint2D32f>::iterator it3 = window.trajectory.begin();

	    while (it3 != window.trajectory.end()) {
	      point.x += it3->x;
	      point.y += it3->y;
	      it3++;
	    }
	    point.x = point.x / mWindowSize;
	    point.y = point.y / mWindowSize;
	    if (window.trajectory.size() != 0) {
	      outputIterator->AddPoint(point);
	      Particle p;
	      p.mCenter.x = point.x;
	      p.mCenter.y = point.y;
	      p.mID = outputIterator->mID;
	      p.mArea = -1;
	      p.mCompactness = -1;
	      p.mOrientation = -1;
	      p.mIDCovariance = -1;
	      mParticles.push_back(p);
	    }
	  } else if (window.trajectory.size() > mWindowSize) {
	    AddError(wxT("Too many points in the window."));
	  }
	  outputIterator++;
	}
	mCore->mDataStructureTracks.mTracks = &mOutputTracks;
	mCore->mDataStructureParticles.mParticles = &mParticles;
	// Let the DisplayImage know about our image
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) {
		de.SetMainImage(mCore->mDataStructureInput.mImage);
		de.SetTrajectories(true);
	}
}

void THISCLASS::OnStepCleanup() {
	mCore->mDataStructureParticles.mParticles = 0;
}

void THISCLASS::OnStop() {

}

void THISCLASS::OnReloadConfiguration()
{

}
