#include "ComponentNearestNeighborTracking.h"
#define THISCLASS ComponentNearestNeighborTracking

#include <sstream>
#include "DisplayEditor.h"

THISCLASS::ComponentNearestNeighborTracking(SwisTrackCore *stc):
		Component(stc, "Tracking"),
		mMaxNumber(10),
		mDisplayOutput("Output", "Tracking") 
{
	// Data structure relations
	mCategory=&(mCore->mCategoryTracking);
	AddDataStructureRead(&(mCore->mDataStructureParticles));
	AddDataStructureWrite(&(mCore->mDataStructureTracks));
	AddDisplay(&mDisplayOutput);

	Initialize();						// Read the XML configuration file
}

THISCLASS::~ComponentNearestNeighborTracking() 
{
	if(mTracks.size()) mTracks.clear();
}

void THISCLASS::OnStart() 
{
	mMaxNumber=GetConfigurationInt("MaxNumber", 10);

	// Check for stupid configurations
	if (mMaxNumber<1)
		AddError("Max number of tracks must be greater or equal to 1");

	for (int i=0;i<mMaxNumber;i++)		// initiate mMaxNumber Track classes
	{
		mTracks.push_back(Track(i,		// id number
			mMaxNumber));
		mTracks.at(i).AddPoint(cvPoint2D32f(320,240));
	}
}

void THISCLASS::OnReloadConfiguration() {
}

void THISCLASS::OnStep() 
{
	// get the particles as input to component
	//	(pointer modifies data in place!)
	particles = mCore->mDataStructureParticles.mParticles;

	// associate all points with the nearest track
	DataAssociation();

	// update the tracks store locally to this component
	mCore->mDataStructureTracks.mTracks=&mTracks;

	// Let the DisplayImage know about our image
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) {
		//de.SetParticles(particles);	//TODO: what was this?
		de.SetMainImage(mCore->mDataStructureInput.mImage);
		de.SetTrajectories(true);
	}
}

void THISCLASS::OnStepCleanup() {
	mCore->mDataStructureParticles.mParticles=0;
}

void THISCLASS::OnStop() {
}

void THISCLASS::DataAssociation()
{
	double min_dist;
	int min_dist_t;

	for(DataStructureParticles::		// for each input particle
		tParticleVector::				//	(blob selection has already
		iterator p=particles->begin();	//	removed any "noise," so there
		p != particles->end();			//	should not be any unwanted 
		p++)							//  particles in the list)
	{
		assert(p->mID == -1);			// (particle should not be associated)

		min_dist = 
			GetCost(0,p->mCenter);
		for(int t=1; t<mMaxNumber; t++)	// compare to all existing tracks
		{
			double dist = 				// and find the one which is 
				GetCost(t,p->mCenter);	//  closest to this particle
			if(dist < min_dist) 
			{
				min_dist = dist;		// and then save both the distance
				min_dist_t = t;			//	and the track number
			}
		}
		p->mID = min_dist_t;			// finally, assign trackID in particle
		AddPoint(min_dist_t,p->mCenter);//  and add particle to track listing
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
double THISCLASS::GetCost(int id,CvPoint2D32f p){

	return(
		(mTracks.at(id).trajectory.back().x-p.x)*
		(mTracks.at(id).trajectory.back().x-p.x)
		+
		(mTracks.at(id).trajectory.back().y-p.y)*
		(mTracks.at(id).trajectory.back().y-p.y)
		);
}



/** Add a point to the current track (max track) 
*
* \param i : Identifies the trajectory p will be added to
* \param p : Point to add to trajectory i (subpixel accuracy)
*/
void THISCLASS::AddPoint(int i, CvPoint2D32f p){
	mTracks.at(i).AddPoint(p);
}
