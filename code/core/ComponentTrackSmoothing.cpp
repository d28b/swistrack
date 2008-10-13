#include "ComponentTrackSmoothing.h"
#define THISCLASS ComponentTrackSmoothing

using namespace std;
#include <iostream>

#include "DisplayEditor.h"

THISCLASS::ComponentTrackSmoothing(SwisTrackCore *stc):
		Component(stc, wxT("TrackSmoothing")),
		mDisplayOutput(wxT("Output"), wxT("Tracking"))
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
  if (mInputTracks.size()) {
    mInputTracks.clear();
  }
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
  return mWindows.back();


}
void THISCLASS::OnStart()
{
	mWindowSize = GetConfigurationInt(wxT("WindowSize"), 3);
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

	//For each track, write data in the corresponding output file
	DataStructureTracks::tTrackVector::iterator it = tracks->begin();
	while (it != tracks->end())
	{
	  Track & window = WindowForTrack(it->mID);
	  cout << "Got track " << window.mID << endl;
	  it++;
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
