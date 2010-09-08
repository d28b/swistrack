#include "ComponentDynamicNearestNeighborTracking.h"
#define THISCLASS ComponentDynamicNearestNeighborTracking

#include "DisplayEditor.h"
#include "Utility.h"
#include <iostream>
#include <set>

using namespace std;
typedef  DataStructureTracks::tTrackMap::value_type tTrackPair;

THISCLASS::ComponentDynamicNearestNeighborTracking(SwisTrackCore *stc):
		Component(stc, wxT("DynamicNearestNeighborTracking")),
		mNextTrackId(0),
		mDisplayOutput(wxT("Output"), wxT("Dynamic Tracking")),
		mTmp1(0), mTmp2(0)

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
	maxParticles = GetConfigurationInt(wxT("MaxNumber"), 10);

	cout << " restarting " << endl;
	mTracks.clear();	// handle reset properly

	THISCLASS::OnReloadConfiguration();
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

	mColorSimilarityThreshold = 
	  GetConfigurationDouble(wxT("ColorSimilarityThreshold"), 0.5);

}

void THISCLASS::OnStep()
{
	wxTimeSpan timeSinceLastFrame = mCore->mDataStructureInput.TimeSinceLastFrame();
	if (timeSinceLastFrame.IsLongerThan(wxTimeSpan::Seconds(5))) {
		cout << "Clearing tracks because there was a gap: " << timeSinceLastFrame.Format().ToAscii() << endl;
		ClearTracks();
	}

	//distance array is too small, release and recreate
	if (mCore->mDataStructureParticles.mParticles->size() > maxParticles)
	{
		maxParticles = mCore->mDataStructureParticles.mParticles->size();
		ClearDistanceArray();

		for (DataStructureTracks::tTrackMap::iterator i = mTracks.begin();
		        i != mTracks.end(); i++)
		{
			squareDistanceArray[i->first] = new double[maxParticles];
		}
	}
	// get the particles as input to component
	//	(pointer modifies data in place!)


	// associate all points with the nearest track
	DataAssociation(mCore->mDataStructureParticles.mParticles);
	
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
	for (map<int, double*>::iterator pos = squareDistanceArray.begin();
	        pos != squareDistanceArray.end(); ++pos) {
		delete[] pos->second;
	}
	squareDistanceArray.clear();
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
			double cost = Utility::SquareDistance(track1.trajectory.back(), track2.trajectory.back());
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

void THISCLASS::DataAssociation(DataStructureParticles::tParticleVector * particles)
{
	int p = 0;

	//for each input particle, blob selection has already removed any wxT("noise"), so there should not be any unwanted particles in the list
	for (DataStructureParticles::tParticleVector::iterator pIt = particles->begin();pIt != particles->end();pIt++, p++)
	  {
		assert(pIt->mID == -1);			// (particle should not be associated)
		for (DataStructureTracks::tTrackMap::iterator i = mTracks.begin();
		        i != mTracks.end(); i++)
		{
			squareDistanceArray[i->first][p] = GetCost(i->second, pIt->mCenter);
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

	if (mTracks.empty() && !particleIndexes.empty()) {
	  int id = mNextTrackId++;
	  mTracks.insert(tTrackPair(id, Track(id)));
	  squareDistanceArray[id] = new double[maxParticles];
	  AddParticle(id, &particles->at(0));
	  particleIndexes.erase(particleIndexes.begin());
	}

	//Search for the minimalDistance
	while ((!trackIndexes.empty()) && (!particleIndexes.empty()))
	{
		double minDistanceSquared;
		int minDistanceI, minDistanceJ;
		minDistanceI = 0;
		minDistanceJ = 0;
		minDistanceSquared = squareDistanceArray[trackIndexes[0]][particleIndexes[0]];
		for (unsigned int i = 0;i < trackIndexes.size();i++)
		{
			for (unsigned int j = 0;j < particleIndexes.size();j++)
			{
				if (squareDistanceArray[trackIndexes[i]][particleIndexes[j]] < minDistanceSquared)
				{
					minDistanceSquared = squareDistanceArray[trackIndexes[i]][particleIndexes[j]];
					minDistanceI = i;
					minDistanceJ = j;
				}
			}
		}
		//If the distance between track and particle is too big, make a new track
		Track * track = NULL;
		Particle * p = &(particles->at(particleIndexes[minDistanceJ]));
		if (minDistanceSquared > mMaxDistanceSquared) {
			if (minDistanceSquared  >= mMinNewTrackDistanceSquared) {
				int id = mNextTrackId++;
				mTracks.insert(tTrackPair(id, Track(id)));
				track = &mTracks[id];
				squareDistanceArray[track->mID] = new double[maxParticles];
				AddParticle(track->mID, p);
			}
		} else {

		  track = &mTracks[trackIndexes[minDistanceI]];
		  if (ColorsMatch(*track, *p)) {
		    AddParticle(track->mID, p);
		  }

		}
		// Suppress the indexes in the vectors		
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
		return Utility::SquareDistance(track.trajectory.back(), p);
	}
}

/** Add a point to the current track (max track)
*
* \param i : Identifies the trajectory p will be added to
* \param p : Point to add to trajectory i (subpixel accuracy)
*/
void THISCLASS::AddParticle(int i, Particle * p){
	Track & track = mTracks[i];
	p->mID = track.mID;
	assert(i == track.mID);
	track.AddPoint(p->mCenter, mCore->mDataStructureInput.mFrameNumber);
	printf("%d adding particle size %.3f\n", track.mID, p->mArea);

	// if somebody doesn't have a color model, then ignore.
	if (track.mColorModel == NULL && p->mColorModel != NULL) {
	  cvCopyHist(p->mColorModel, &track.mColorModel);
	}else if (p->mColorModel != NULL && track.mColorModel != NULL) {
	  Utility::IntegrateHistogram(track.mColorModel, p->mColorModel);
	  cvCopyHist(p->mColorModel, &track.mColorModel);
	} else {
	  // no color features.
	}
}


void THISCLASS::ClearTracks() {
	mTracks.clear();
}


bool THISCLASS::ColorsMatch(const Track & track, const Particle & particle) {

  if (particle.mColorModel == NULL || track.mColorModel == NULL) {
    return true;
  }

  cvCopyHist(track.mColorModel, &mTmp1);
  cvCopyHist(particle.mColorModel, &mTmp2);

  cvNormalizeHist(mTmp1, 1);
  cvNormalizeHist(mTmp2, 1);

  
  
  double colorSim = cvCompareHist(mTmp1, mTmp2, CV_COMP_BHATTACHARYYA); 
  printf("%d Color theshold: %.4f <= %.4f\n", 
	 track.mID, colorSim, mColorSimilarityThreshold);
  if (colorSim <= mColorSimilarityThreshold) {
    return true;
  } else {
    printf("Rejecting because the color is different: %.4f\n", colorSim);
    return false;
  }
  
}
