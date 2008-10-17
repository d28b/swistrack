#include "ComponentOpticalFlowPyrLKTracking.h"
#define THISCLASS ComponentOpticalFlowPyrLKTracking

using namespace std;
#include <iostream>
#include <set>

#include "DisplayEditor.h"

THISCLASS::ComponentOpticalFlowPyrLKTracking(SwisTrackCore *stc):
		Component(stc, wxT("OpticalFlowPyrLK")),
		mLastFrame(0), mOutputFrame(0),
		mCorners(0), mCornersB(0), eig_image(0), tmp_image(0),
		pyrA(0), pyrB(0),
		mNextTrackId(0),
		mDisplayOutput(wxT("Output"), wxT("Optical Flow PyrLKT Tracking"))
{
	// Data structure relations
	mCategory = &(mCore->mCategoryTracking);

	AddDataStructureRead(&(mCore->mDataStructureParticles));
	AddDataStructureWrite(&(mCore->mDataStructureParticles));
	AddDataStructureWrite(&(mCore->mDataStructureTracks));
	AddDisplay(&mDisplayOutput);

	Initialize();						// Read the XML configuration file
}

THISCLASS::~ComponentOpticalFlowPyrLKTracking()
{
  if (eig_image) {
    cvReleaseImage(&eig_image);
  }
  if (tmp_image) {
    cvReleaseImage(&tmp_image);
  } 
  if (mLastFrame) {
    cvReleaseImage(&mLastFrame);
  }
  if (mOutputFrame) {
    cvReleaseImage(&mOutputFrame);
  }
  if (mFeaturesFound) {
    free(mFeaturesFound);
    mFeaturesFound = NULL;
  }
  if (mFeatureErrors) {
    free(mFeatureErrors);
    mFeatureErrors = NULL;
  }
  if (mCorners) {
    delete[] mCorners;
    mCorners = 0;
  }
  if (mCornersB) {
    delete[] mCorners;
    mCorners = 0;
  }
     
  if (mFeaturesFound) {
  }
	mTracks.clear();
	ClearDistanceArray();
}

void THISCLASS::OnStart()
{
	maxParticles = 10;

	cout << " restarting " << endl;
	mTracks.clear();	// handle reset properly

	THISCLASS::OnReloadConfiguration();
}

void THISCLASS::InitializeTracks() 
{
  int id = mNextTrackId++;
  mTracks[id] = Track(id);
  distanceArray[id] = new double[maxParticles];
}

void THISCLASS::OnReloadConfiguration()
{
	mMaxDistanceSquared = GetConfigurationDouble(wxT("MaxDistance"), 10);
	mMaxDistanceSquared *= mMaxDistanceSquared;
	
	mMinNewTrackDistanceSquared = 
	  pow(GetConfigurationDouble(wxT("MinNewTrackDistance"), 10), 2);

	mFrameKillThreshold = 
	  GetConfigurationDouble(wxT("FrameKillThreshold"), 10);
	mTrackDistanceKillThresholdSquared = 
	  pow(GetConfigurationDouble(wxT("TrackDistanceKillThreshold"), 10), 2);

	mMaxCorners = 500;
	mCornerCount = mMaxCorners;
	mWinSize = 10;
	mCorners = new CvPoint2D32f[mMaxCorners];
	mCornersB = new CvPoint2D32f[mMaxCorners];

	mFeaturesFound = (char *) malloc(sizeof(char *) * mMaxCorners);
	mFeatureErrors = (float *) malloc(sizeof(float *) * mMaxCorners);

	

	InitializeTracks();
}

void THISCLASS::OnStep()
{
  	IplImage *thisFrame = mCore->mDataStructureImageColor.mImage;
	if (mThisFrame == NULL) {
	  mThisFrame = cvCreateImage(cvSize(thisFrame->width,
					    thisFrame->height),
				     thisFrame->depth, 1);
	}
	cvCvtColor(thisFrame, mThisFrame, CV_BGR2GRAY);
	
	if (mOutputFrame == NULL) {
	  mOutputFrame = cvCreateImage(cvSize(mThisFrame->width, 
					      mThisFrame->height), 
				       mThisFrame->depth, mThisFrame->nChannels);
	}
	if (mLastFrame == NULL) {
	  mLastFrame = cvCreateImage(cvSize(mThisFrame->width, 
					    mThisFrame->height), 
				     mThisFrame->depth, mThisFrame->nChannels);
	  cvCopy(mThisFrame, mLastFrame);
	  cout << "Bailing because last frame is null." << mLastFrame << endl;
	  return;
	}

	if (eig_image == NULL) {
	  eig_image = cvCreateImage(cvGetSize(mThisFrame), IPL_DEPTH_32F, 1);
	}
	if (tmp_image == NULL) {
	  eig_image = cvCreateImage(cvGetSize(mThisFrame), IPL_DEPTH_32F, 1);
	}


	cvGoodFeaturesToTrack(mThisFrame, eig_image, tmp_image,
			      mCorners,
			      &mCornerCount, 
			      0.01, 5.0, 0, 3, 0, 0.04); 
	cout << "Got " << mCornerCount << " corners." << endl;
	cvFindCornerSubPix(mLastFrame, mCorners, mCornerCount,
			   cvSize(mWinSize, mWinSize),
			   cvSize(-1, -1),
			   cvTermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,
					  20, 0.03));
					  
	CvSize pyr_size = cvSize(mThisFrame->width + 8, mThisFrame->height / 3);
	
	if (pyrA == NULL) {
	  pyrA = cvCreateImage(pyr_size, IPL_DEPTH_32F, 1);
	}
	if (pyrB == NULL) {
	  pyrB = cvCreateImage(pyr_size, IPL_DEPTH_32F, 1);
	}
	cout << "Flowing " << endl;
	cvCalcOpticalFlowPyrLK(mLastFrame, mThisFrame,
			       pyrA, pyrB, 
			       mCorners, mCornersB,
			       mCornerCount, 
			       cvSize(mWinSize, mWinSize),
			       5,
			       mFeaturesFound,
			       mFeatureErrors,
			       cvTermCriteria(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS,
					      20, 0.3),
			       0);
	cvCopy(mThisFrame, mOutputFrame);
	for (int i = 0; i < mCornerCount; i++) {
	  if (mFeaturesFound[i] == 0 || mFeatureErrors[i] > 550) {
	    printf("Error is %f\n", mFeatureErrors[i]);
	  } else {
	    CvPoint p0 = cvPoint(cvRound(mCorners[i].x),
				 cvRound(mCorners[i].y));
	    CvPoint p1 = cvPoint(cvRound(mCornersB[i].x),
				 cvRound(mCornersB[i].y));
	    cvLine(mOutputFrame, p0, p1, CV_RGB(255, 255, 255), 2);
	      
	  }
	  
	}
	mCore->mDataStructureTracks.mTracks = &mTracks;	       
	cvCopy(mThisFrame, mLastFrame);
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) {
	  cout << " Sending output frame " << endl;
	  de.SetMainImage(mOutputFrame);
	  de.SetTrajectories(true);
	}
}

void THISCLASS::OnStepCleanup() {
	mCore->mDataStructureParticles.mParticles = 0;
}

void THISCLASS::ClearDistanceArray() {
  for (map<int, double*>::iterator pos = distanceArray.begin();
       pos != distanceArray.end(); ++pos) {
    delete[] pos->second;
  }
  distanceArray.clear();
}

void THISCLASS::OnStop() {
  ClearDistanceArray();

}
void THISCLASS::FilterTracks() 
{
  
  for (DataStructureTracks::tTrackMap::iterator i = mTracks.begin();
       i != mTracks.end(); i++) {
    Track & track = i->second;
    if (mCore->mDataStructureInput.mFrameNumber - track.LastUpdateFrame() >= mFrameKillThreshold) {
      cout << "Erasing (updated)" << track.mID << endl; 
      mTracks.erase(i);

    }
  }

  set<int> trackIdsToErase;
  for (DataStructureTracks::tTrackMap::iterator i = mTracks.begin();
       i != mTracks.end(); i++) {
    Track & track1 = i->second;
    for (DataStructureTracks::tTrackMap::iterator j = mTracks.begin();
	 j != mTracks.end(); j++) {
      Track & track2 = j->second;
      double cost = squareDistance(track1.trajectory.back(), track2.trajectory.back());
      if (track1.mID != track2.mID && cost < mTrackDistanceKillThresholdSquared) {
	// kill a track - keep the older one
	if (track1.mID < track2.mID) {
	  trackIdsToErase.insert(track2.mID);
	  cout << "Erasing (close to " << track1.mID << ")" << track2.mID << endl;
	} else {
	  trackIdsToErase.insert(track1.mID);
	  cout << "Erasing (close to " << track2.mID << ")" << track1.mID << endl;
	  break;
	}
      }
    }
  }
  for (set<int>::iterator i = trackIdsToErase.begin();
       i != trackIdsToErase.end(); i++) {

    mTracks.erase(*i);
  }
  
}

void THISCLASS::DataAssociation()
{
	int p = 0;
	//for each input particle, blob selection has already removed any wxT("noise"), so there should not be any unwanted particles in the list
	for (DataStructureParticles::tParticleVector::iterator pIt = particles->begin();pIt != particles->end();pIt++, p++)
	{
		assert(pIt->mID == -1);			// (particle should not be associated)
		for (DataStructureTracks::tTrackMap::iterator i = mTracks.begin();
		     i != mTracks.end(); i++)
		{
			distanceArray[i->first][p] = GetCost(i->second, pIt->mCenter);
		}
		//  Compute distance from each particle to each track
	}
	//Now, we have all the distances between the track and the particles.
	//Register the existing indexes
	std::vector<int> trackIndexes;
	std::vector<int> particleIndexes;
	for (unsigned int i = 0;i < particles->size();i++) {
		particleIndexes.push_back(i);
	}
	for (DataStructureTracks::tTrackMap::iterator i = mTracks.begin();
	     i != mTracks.end(); i++) {
	  trackIndexes.push_back(i->first);
	}
	//Search for the minimalDistance
	while ((!trackIndexes.empty()) && (!particleIndexes.empty()))
	{
		double minDistance;
		int minDistanceI, minDistanceJ;
		minDistanceI = 0;
		minDistanceJ = 0;
		minDistance = distanceArray[trackIndexes[0]][particleIndexes[0]];
		for (unsigned int i = 0;i < trackIndexes.size();i++)
		{
			for (unsigned int j = 0;j < particleIndexes.size();j++)
			{
				if (distanceArray[trackIndexes[i]][particleIndexes[j]] < minDistance)
				{
					minDistance = distanceArray[trackIndexes[i]][particleIndexes[j]];
					minDistanceI = i;
					minDistanceJ = j;
				}
			}
		}
		//If the distance between track and particle is too big, make a new track
		Track * track = NULL;
		if (minDistance > mMaxDistanceSquared) {
		  if (minDistance  >= mMinNewTrackDistanceSquared) {
		    int id = mNextTrackId++;
		    mTracks[id] =  Track(id);

		    track = &mTracks[id];
		    distanceArray[track->mID] = new double[maxParticles];
		    cout << " Making a new track:  " << track->mID << " distance " << minDistance << endl;
		  } else {
		    break;
		  }
		} else {
		  track = &mTracks.at(trackIndexes[minDistanceI]);
		}
		    
		(particles->at(particleIndexes[minDistanceJ])).mID = track->mID;
		AddPoint(track->mID, 
			 particles->at(particleIndexes[minDistanceJ]).mCenter);

		//Suppress the indexes in the vectors
		trackIndexes.erase(trackIndexes.begin() + minDistanceI);
		particleIndexes.erase(particleIndexes.begin() + minDistanceJ);
	}
}



/** Calculates cost for two points to be associated.
* Here: cost is distance.
*
* \param id : the id of the trajectory
* \param p  : a point
*
* \return Cost between a trajectory and a point
* \todo The cost function used here is very simple. One could imagine to take also other
* attributes, for instance the speed of the object into account.
*/
double THISCLASS::GetCost(const Track & track, CvPoint2D32f p)
{
	if (track.trajectory.size() == 0)
		return -1;
	else
	{
                return squareDistance(track.trajectory.back(), p);
	}
}

/** Add a point to the current track (max track)
*
* \param i : Identifies the trajectory p will be added to
* \param p : Point to add to trajectory i (subpixel accuracy)
*/
void THISCLASS::AddPoint(int i, CvPoint2D32f p){
  Track & track = mTracks.at(i);
  assert(i == track.mID);
  track.AddPoint(p, mCore->mDataStructureInput.mFrameNumber);
}
