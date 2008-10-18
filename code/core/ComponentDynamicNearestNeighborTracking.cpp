#include "ComponentDynamicNearestNeighborTracking.h"
#define THISCLASS ComponentDynamicNearestNeighborTracking

using namespace std;
#include <iostream>
#include <set>

#include "DisplayEditor.h"

THISCLASS::ComponentDynamicNearestNeighborTracking(SwisTrackCore *stc):
		Component(stc, wxT("DynamicNearestNeighborTracking")),
		mNextTrackId(0),
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

THISCLASS::~ComponentDynamicNearestNeighborTracking()
{
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


	InitializeTracks();
}

void THISCLASS::OnStep()
{
	//distance array is too small, release and recreate
	if (mCore->mDataStructureParticles.mParticles->size() > maxParticles)
	{
		maxParticles = mCore->mDataStructureParticles.mParticles->size();
		ClearDistanceArray();

		for (DataStructureTracks::tTrackMap::iterator i = mTracks.begin();
		     i != mTracks.end(); i++) 
		{
		  distanceArray[i->first] = new double[maxParticles];
		}
	}
	// get the particles as input to component
	//	(pointer modifies data in place!)
	particles = mCore->mDataStructureParticles.mParticles;
	if (mTracks.size() == 0) {
	  InitializeTracks();
	}

	// associate all points with the nearest track
	DataAssociation();

	// get rid of the tracks that have died.
	FilterTracks();

	// update the tracks store locally to this component
	mCore->mDataStructureTracks.mTracks = &mTracks;

	// Let the DisplayImage know about our image
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) {
		//de.SetParticles(particles);	//TODO: what was this?
		de.SetMainImage(mCore->mDataStructureInput.mImage);
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
	} else {
	  trackIdsToErase.insert(track1.mID);
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

double squareDistance(CvPoint2D32f p1, CvPoint2D32f p2)
 {
  double dx = p1.x - p2.x;
  double dy = p1.y - p2.y;
  return dx*dx + dy*dy;
}
