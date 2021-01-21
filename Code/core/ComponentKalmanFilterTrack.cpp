#include "ComponentKalmanFilterTrack.h"
#define THISCLASS ComponentKalmanFilterTrack

using namespace std;
#include <iostream>

#include "DisplayEditor.h"

THISCLASS::ComponentKalmanFilterTrack(SwisTrackCore *stc):
		Component(stc, wxT("KalmanFilterTrack")),
		mDisplayOutput(wxT("Output"), wxT("Smoothing"))
{
	// Data structure relations
	mCategory = &(mCore->mCategoryTracking);
	AddDataStructureRead(&(mCore->mDataStructureTracks));
	AddDataStructureWrite(&(mCore->mDataStructureTracks));
	AddDisplay(&mDisplayOutput);

	Initialize(); // Read the XML configuration file

}

THISCLASS::~ComponentKalmanFilterTrack()
{
  if (mOutputTracks.size()) {
    mOutputTracks.clear();
  }
}
Track& THISCLASS::GetOrMakeTrack(int id, CvPoint2D32f p) 
{
  if (mOutputTracks.find(id) != mOutputTracks.end()) {
    return mOutputTracks[id];
  } else {
    mOutputTracks[id] = Track(id);
    mOutputTracks[id].SetMaxLength(mWindowSize);
    mOutputTracks[id] = Track(id);
    
    mKalman[id] = cvCreateKalman(4, 4, 0);

    mZ_k[id] = cvCreateMat(4, 1, CV_32FC1); // measurments
    cvZero(mZ_k[id]);
    mF[id] = cvCreateMat(4, 4, CV_32FC1);
    cvSetIdentity(mF[id], cvRealScalar(1));

    cvSetIdentity(mKalman[id]->measurement_matrix, cvRealScalar(1));
    cvSetIdentity(mKalman[id]->process_noise_cov, cvRealScalar(1e-1));
    cvSetIdentity(mKalman[id]->measurement_noise_cov, cvRealScalar(1e-1));
    cvSetIdentity(mKalman[id]->error_cov_post, cvRealScalar(1));

    cvSetReal1D(mKalman[id]->state_post, 0, p.x);
    cvSetReal1D(mKalman[id]->state_post, 1, p.y);
    cvSetReal1D(mKalman[id]->state_post, 2, 0);
    cvSetReal1D(mKalman[id]->state_post, 3, 0);


    
    return mOutputTracks[id];
  }


}

CvPoint2D32f THISCLASS::StepFilter(int id, CvPoint2D32f newMeasurement, CvPoint2D32f * lastMeasurement) {
  wxTimeSpan frameInterval = mCore->mDataStructureInput.FrameTimestamp() - mLastTs;
  double dt = frameInterval.GetMilliseconds().ToDouble() * 1000.0;
  cvSetReal2D(mF[id], 0, 2, dt);
  cvSetReal2D(mF[id], 1, 3, dt);
  
  const CvMat * y_k = cvKalmanPredict(mKalman[id], 0);


  cvSetReal1D(mZ_k[id], 0, newMeasurement.x);
  cvSetReal1D(mZ_k[id], 1, newMeasurement.y);

  if (lastMeasurement != NULL) {
    cvSetReal1D(mZ_k[id], 2, (newMeasurement.x - lastMeasurement->x) / dt);
    cvSetReal1D(mZ_k[id], 3, (newMeasurement.y - lastMeasurement->y) / dt);
  }
  
  //  cout << "Position: " << 
  //cvGetReal1D(mKalman[id]->state_post, 0)  << ", " <<
  //cvGetReal1D(mKalman[id]->state_post, 1)  << endl;

  //  cout << "Velocity: " << 
  //cvGetReal1D(mKalman[id]->state_post, 2)  << ", " <<
  //cvGetReal1D(mKalman[id]->state_post, 3)  << endl;
  cvKalmanCorrect(mKalman[id], mZ_k[id]);
  return cvPoint2D32f(cvGetReal1D(y_k, 0), cvGetReal1D(y_k, 1));
}

void THISCLASS::OnStart()
{
  mWindowSize = GetConfigurationInt(wxT("WindowSize"), 3);
  mOutputTracks.clear();      

}

void THISCLASS::EraseTrack(int id) {
  
  cvReleaseKalman(&mKalman[id]);
  mKalman.erase(id);


  cvReleaseMat(&mZ_k[id]);
  mZ_k.erase(id);

  cvReleaseMat(&mF[id]);
  mF.erase(id);
}


void THISCLASS::OnStep()
{
  DataStructureTracks::tTrackMap *tracks = 
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
	for (DataStructureTracks::tTrackMap::iterator i = mOutputTracks.begin();
	     i != mOutputTracks.end(); i++)
	  {
	    if (tracks->find(i->first) == tracks->end()) {
	      EraseTrack(i->first);
	      mOutputTracks.erase(i); // os x doesn't like when we erase by index in EraseTracks.
	    }
	  }
	mParticles.clear();
	//For each track, write data in the corresponding output file
	for (DataStructureTracks::tTrackMap::iterator it = tracks->begin();
	     it != tracks->end(); it++) {

	  //Search for the corresponding particle
	  for (DataStructureParticles::tParticleVector::iterator it2 = particles->begin();
	       it2 != particles->end(); it2++) {
	    if (it->first == it2->mID) {
	      Track & t = GetOrMakeTrack(it->first, it2->mCenter);
	      CvPoint2D32f * lastPoint = NULL;
	      if (t.trajectory.size() != 0) {
		lastPoint = &t.trajectory.back();
	      }
	      CvPoint2D32f newPoint = StepFilter(t.mID, it2->mCenter, lastPoint);
	      Particle p = *it2;
	      p.mCenter = newPoint;
	      t.AddPoint(newPoint, mCore->mDataStructureInput.mFrameNumber);
	      mParticles.push_back(p);
	      
	    }
	  }
	}

	mCore->mDataStructureTracks.mTracks = &mOutputTracks;
	mCore->mDataStructureParticles.mParticles = &mParticles;
	// Let the DisplayImage know about our image
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) {
		de.SetMainImage(mCore->mDataStructureInput.mImage);
		de.SetTrajectories(true);
	}

	mLastTs = mCore->mDataStructureInput.FrameTimestamp();
}


void THISCLASS::OnStepCleanup() {
	mCore->mDataStructureParticles.mParticles = 0;
}

void THISCLASS::OnStop() {

}

void THISCLASS::OnReloadConfiguration()
{

}
