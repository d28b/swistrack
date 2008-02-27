#include "ComponentNearestNeighborTracking.h"
#define THISCLASS ComponentNearestNeighborTracking

#include <sstream>
#include "DisplayEditor.h"

THISCLASS::ComponentNearestNeighborTracking(SwisTrackCore *stc):
		Component(stc, "NearestNeighborTracking"),
		mMaxNumber(10),
		mDisplayOutput("Output", "Tracking") 
{
	// Data structure relations
	mCategory=&(mCore->mCategoryTracking);
	AddDataStructureRead(&(mCore->mDataStructureParticles));
	AddDataStructureWrite(&(mCore->mDataStructureParticles));
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
	maxParticles=mCore->mDataStructureParticles.mParticles->size();

	// Check for stupid configurations
	if (mMaxNumber<1)
		AddError("Max number of tracks must be greater or equal to 1");

	if(mTracks.size()) mTracks.clear();	// handle reset properly
	for (int i=0;i<mMaxNumber;i++)		// initiate mMaxNumber Track classes
	{
		mTracks.push_back(Track(i,		// id number
			mMaxNumber));
		//mTracks.at(i).AddPoint(cvPoint2D32f(320,240));
	}
	distanceArray= new double*[mMaxNumber];
	for (int i=0;i<mMaxNumber;i++)
		distanceArray[i]=new double[maxParticles];	
}

void THISCLASS::OnReloadConfiguration() {
}

void THISCLASS::OnStep() 
{
	//distance array is too small, release and recreate
	if (mCore->mDataStructureParticles.mParticles->size()>maxParticles)
	{
		maxParticles=mCore->mDataStructureParticles.mParticles->size();
		for (int i=0;i<mMaxNumber;i++)
		{
			delete[] distanceArray[i];
			distanceArray[i]=new double[maxParticles];
		}
	}
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
	for (int i;i=0;i++)
		delete[] distanceArray[i];	
}

void THISCLASS::DataAssociation()
{	
	int p=0;
	//for each input particle, blob selection has already removed any "noise", so there should not be any unwanted particles in the list
	for(DataStructureParticles::tParticleVector::iterator pIt=particles->begin();pIt != particles->end();pIt++,p++)	
	{
		assert(pIt->mID == -1);			// (particle should not be associated)		
		for(int t=0; t<mMaxNumber; t++)	// compare to all existing tracks
		{
			distanceArray[t][p]=GetCost(t,pIt->mCenter);
		}
										//  Compute distance from each particle to each track			
	}
	//Now, we have all the distances between the track and the particles.
	//Register the existing indexes
	std::vector<int> trackIndexes;
	std::vector<int> particleIndexes;
	for (int i=0;i<particles->size();i++)
		particleIndexes[i]=i;
	for (int i=0;i<mMaxNumber;i++)
		trackIndexes[i]=i;
	//Search for the minimalDistance
	while ((!trackIndexes.empty())&&(!particleIndexes.empty()))
	{
		double minDistance;
		int minDistanceI,minDistanceJ;
		minDistanceI=0;
		minDistanceJ=0;
		minDistance=distanceArray[0][0];
		for (int i=0;i<trackIndexes.size();i++)
		{
			for (int j=0;j<particles->size();j++)
			{
				if (distanceArray[i][j]<minDistance)
				{
					minDistance=distanceArray[i][j];
					minDistanceI=i;
					minDistanceJ=j;
				}
			}
		}
		//The smallest distance into the array is for i,j
		(particles->at(minDistanceJ)).mID=minDistanceI;
		AddPoint(minDistanceI,(particles->at(minDistanceJ)).mCenter);
		//Suppress the indexes in the vectors
		for (int i=0;;i++)
		{
			if (trackIndexes[i]=minDistanceI)
			{
				trackIndexes.erase(trackIndexes.begin()+i);
				break;
			}
		}
		for (int j=0;;j++)
		{
			if (trackIndexes[j]=minDistanceJ)
			{
				particleIndexes.erase(particleIndexes.begin()+j);
				break;
			}
		}
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
double THISCLASS::GetCost(int id,CvPoint2D32f p)
{
	if(mTracks.at(id).trajectory.size()==0)
		return -1;
	else
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
