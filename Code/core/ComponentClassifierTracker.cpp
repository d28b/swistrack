#include "ComponentClassifierTracker.h"
#define THISCLASS ComponentClassifierTracker

#include "DisplayEditor.h"
#include "Utility.h"
#include <iostream>
#include <set>

using namespace std;
typedef  DataStructureTracks::tTrackMap::value_type tTrackPair;

THISCLASS::ComponentClassifierTracker(SwisTrackCore *stc):
		Component(stc, wxT("ClassifierTracker")),
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

THISCLASS::~ComponentClassifierTracker()
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

	DataAssociationClassifier::ExampleTable table = 
	  DataAssociationClassifier::fromFile("../../track.benchmark.training.txt");
	mClassifier.Train(table);
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


