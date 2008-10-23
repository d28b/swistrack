#include "ComponentCamShiftTracking.h"
#define THISCLASS ComponentCamShiftTracking
using namespace std;
#include <iostream>

#include "DisplayEditor.h"

#include "utils.h"

THISCLASS::ComponentCamShiftTracking(SwisTrackCore *stc):
		Component(stc, wxT("CamShiftTracking")),
		mOutputImage(0),
		mDisplayOutput(wxT("Output"), wxT("Tracking"))
{
	// Data structure relations
	mCategory = &(mCore->mCategoryTracking);
	AddDataStructureRead(&(mCore->mDataStructureImageColor));
	AddDataStructureWrite(&(mCore->mDataStructureParticles));
	AddDataStructureWrite(&(mCore->mDataStructureTracks));
	AddDisplay(&mDisplayOutput);

	Initialize();						// Read the XML configuration file
}

THISCLASS::~ComponentCamShiftTracking()
{

}

void THISCLASS::OnStart()
{
  

  setVmin(&cs, GetConfigurationInt(wxT("VMin"), 60));
  setSmin(&cs, GetConfigurationInt(wxT("SMin"), 50));
  //mTracker.set_window(cvRect(452, 493, 75, 110));
  //int hist_sizes[] = {30, 32, 50};
  //mTracker.set_hist_dims(1, hist_sizes);
  //mTracker.set_hist_bin_range(0, 0, 180);
}

void THISCLASS::OnReloadConfiguration()
{
  OnStop();
}
void THISCLASS::UpdateTrackers(IplImage * inputImage)
{
  for (std::map<int, camshift>::iterator i = mTrackers.begin();
       i != mTrackers.end(); i++) {
    CvBox2D box = track(&i->second, inputImage);
    mTracks[i->first].AddPoint(box.center, 
			       mCore->mDataStructureInput.mFrameNumber);
  }
}
void THISCLASS::OnStep()
{	
  IplImage *inputImage = mCore->mDataStructureImageColor.mImage;
  if (! inputImage) {
    AddError(wxT("No input image."));
    return;
  }

  UpdateTrackers(inputImage);

  if (mCore->mDataStructureParticles.mParticles == NULL) {
    AddError(wxT("No input particles!"));
    return;
  }
  const DataStructureParticles::tParticleVector & particles = *mCore->mDataStructureParticles.mParticles;
  for (DataStructureParticles::tParticleVector::const_iterator pIt = 
	 particles.begin();
       pIt != particles.end(); pIt++) {
    assert(pIt->mID == -1); // (particle should not be associated)
    float minSquareDist = std::numeric_limits<float>::max();
    Track * closestTrack = NULL;
    for (DataStructureTracks::tTrackMap::iterator i = mTracks.begin();
	 i != mTracks.end(); i++) {
      float distance = squareDistance(i->second.trajectory.back(), 
				      pIt->mCenter);
      if (distance < minSquareDist) {
	minSquareDist = distance;
	closestTrack = &i->second;
      }
    }
    if (closestTrack == NULL || minSquareDist < mMinNewTrackDistanceSquared) {
      int id = mNextTrackId++;
      mTracks[id] = Track(id);
      mTrackers[id] = camshift();
      createTracker(&mTrackers[id], inputImage);
      CvRect start = rectByCenter(pIt->mCenter.x, 
				  pIt->mCenter.y,
				  10, 10);
      startTracking(&mTrackers[id], inputImage, &start);
      cout << "Adding track " << id << endl;
    }
  }
  if (mOutputImage == 0) {
    //CvRect start = cvRect(452, 493, 75, 110);
    CvRect start = cvRect(452, 493, 20, 20);
    cout << "Restarting tracking." << endl;
    createTracker(&cs, inputImage);
    startTracking(&cs, inputImage, &start);
  }

  CvBox2D box = track(&cs, inputImage);

  // update the tracks store locally to this component
  mCore->mDataStructureTracks.mTracks = &mTracks;
  // Let the DisplayImage know about our image
  DisplayEditor de(&mDisplayOutput);
  if (de.IsActive()) {
    if (mOutputImage != NULL) {
      cvReleaseImage(&mOutputImage);
    }
    mOutputImage = cvCloneImage(inputImage);
    

    for (std::map<int, camshift>::iterator i = mTrackers.begin();
	 i != mTrackers.end(); i++) {    
      cout << "Drawing the track " << 
	i->second.faceBox.center.x << "," << 
	i->second.faceBox.center.y << endl;
      cvEllipseBox(mOutputImage, i->second.faceBox,
		   CV_RGB(255,0,0), 3, CV_AA, 0 );
    }
    cvEllipseBox(mOutputImage, box,
		 CV_RGB(255,0,0), 3, CV_AA, 0 );

    de.SetMainImage(mOutputImage);
    de.SetTrajectories(true);
  }
}

void THISCLASS::OnStepCleanup() {
	mCore->mDataStructureParticles.mParticles = 0;
}

void THISCLASS::OnStop() {
   releaseTracker(&cs);
   cvReleaseImage(&mOutputImage);
   mOutputImage = 0;
}

