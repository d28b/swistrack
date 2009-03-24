#include "ComponentNearestNeighborTracking.h"
#define THISCLASS ComponentNearestNeighborTracking

#include "DisplayEditor.h"

THISCLASS::ComponentNearestNeighborTracking(SwisTrackCore *stc):
		Component(stc, wxT("NearestNeighborTracking")),
		mMaxNumber(10),
		mDisplayOutput(wxT("Output"), wxT("Tracking"))
{
	// Data structure relations
	mCategory = &(mCore->mCategoryTracking);
	AddDataStructureRead(&(mCore->mDataStructureParticles));
	AddDataStructureWrite(&(mCore->mDataStructureParticles));
	AddDataStructureWrite(&(mCore->mDataStructureTracks));
	AddDisplay(&mDisplayOutput);

	Initialize();						// Read the XML configuration file
}

THISCLASS::~ComponentNearestNeighborTracking()
{
	if (mTracks.size()) mTracks.clear();
}

void THISCLASS::OnStart()
{
	mMaxNumber = GetConfigurationInt(wxT("MaxNumber"), 10);
	maxParticles = 10;

	// Check for stupid configurations
	if (mMaxNumber < 1)
		AddError(wxT("Max number of tracks must be greater or equal to 1"));

	if (mTracks.size()) mTracks.clear();	// handle reset properly
	for (int i = 0;i < mMaxNumber;i++)		// initiate mMaxNumber Track classes
	{
		mTracks[i] = Track(i);
		//mTracks.at(i).AddPoint(cvPoint2D32f(320,240));
	}
	distanceArray = new double*[mMaxNumber];
	for (int i = 0;i < mMaxNumber;i++)
		distanceArray[i] = new double[maxParticles];
	THISCLASS::OnReloadConfiguration();
}

void THISCLASS::OnReloadConfiguration()
{
	mMaxDistance = GetConfigurationDouble(wxT("MaxDistance"), 10);
	mMaxDistance *= mMaxDistance;
}

void THISCLASS::OnStep()
{
	//distance array is too small, release and recreate
	if ((int) mCore->mDataStructureParticles.mParticles->size() > maxParticles)
	{
		maxParticles = mCore->mDataStructureParticles.mParticles->size();
		for (int i = 0;i < mMaxNumber;i++)
		{
			delete[] distanceArray[i];
			distanceArray[i] = new double[maxParticles];
		}
	}
	// get the particles as input to component
	//	(pointer modifies data in place!)
	particles = mCore->mDataStructureParticles.mParticles;

	// associate all points with the nearest track
	DataAssociation();

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

void THISCLASS::OnStop() {
	for (int i;i = 0;i++)
		delete[] distanceArray[i];
}

void THISCLASS::DataAssociation()
{
	int p = 0;
	//for each input particle, blob selection has already removed any wxT("noise"), so there should not be any unwanted particles in the list
	for (DataStructureParticles::tParticleVector::iterator pIt = particles->begin();pIt != particles->end();pIt++, p++)
	{
		assert(pIt->mID == -1);			// (particle should not be associated)
		for (int t = 0; t < mMaxNumber; t++)	// compare to all existing tracks
		{
			distanceArray[t][p] = GetCost(t, pIt->mCenter);
		}
		//  Compute distance from each particle to each track
	}
	//Now, we have all the distances between the track and the particles.
	//Register the existing indexes
	std::vector<int> trackIndexes;
	std::vector<int> particleIndexes;
	for (int i = 0;i < (int) particles->size();i++)
		particleIndexes.push_back(i);
	for (int i = 0;i < mMaxNumber;i++)
		trackIndexes.push_back(i);
	//Search for the minimalDistance
	while ((!trackIndexes.empty()) && (!particleIndexes.empty()))
	{
		double minDistance;
		int minDistanceI, minDistanceJ;
		minDistanceI = 0;
		minDistanceJ = 0;
		minDistance = distanceArray[trackIndexes[0]][particleIndexes[0]];
		for (int i = 0;i < (int) trackIndexes.size();i++)
		{
			for (int j = 0;j < (int) particleIndexes.size();j++)
			{
				if (distanceArray[trackIndexes[i]][particleIndexes[j]] < minDistance)
				{
					minDistance = distanceArray[trackIndexes[i]][particleIndexes[j]];
					minDistanceI = i;
					minDistanceJ = j;
				}
			}
		}
		//If the distance between track and particle is too big, allocate nothing.
		if (minDistance > mMaxDistance)
			break;
		//The smallest distance into the array is for i,j
		(particles->at(particleIndexes[minDistanceJ])).mID = trackIndexes[minDistanceI];
		AddPoint(trackIndexes[minDistanceI], (particles->at(particleIndexes[minDistanceJ])).mCenter);
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
double THISCLASS::GetCost(int id, CvPoint2D32f p)
{
	if (mTracks[id].trajectory.size() == 0)
		return -1;
	else
	{
		double dx = mTracks[id].trajectory.back().x - p.x;
		double dy = mTracks[id].trajectory.back().y - p.y;
		return dx*dx + dy*dy;
	}
}

/** Add a point to the current track (max track)
*
* \param i : Identifies the trajectory p will be added to
* \param p : Point to add to trajectory i (subpixel accuracy)
*/
void THISCLASS::AddPoint(int i, CvPoint2D32f p){
       mTracks[i].AddPoint(p, mCore->mDataStructureInput.mFrameNumber);
}
