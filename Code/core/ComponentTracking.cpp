#include "ComponentTracking.h"
#define THISCLASS ComponentTracking

#include "DisplayEditor.h"

THISCLASS::ComponentTracking(SwisTrackCore *stc):
		Component(stc, wxT("Tracking")),
		mShareTrajectories(1), mMaxNumber(10),
		mDistanceGate(0.1), mDisplayOutput(wxT("Output"), wxT("Tracking")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryTracking);
	AddDataStructureRead(&(mCore->mDataStructureParticles));
	AddDataStructureWrite(&(mCore->mDataStructureTracks));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentTracking() {
	if (mTracks.size()) mTracks.clear();
}

void THISCLASS::OnStart() {
	mMaxNumber = GetConfigurationInt(wxT("MaxNumber"), 10);
	mDistanceGate = GetConfigurationDouble(wxT("DistanceGate"), 0.1);
	mShareTrajectories = GetConfigurationInt(wxT("ShareTrajectories"), 1);

	// Check for stupid configurations
	if (mMaxNumber < 1) {
		AddError(wxT("Max number of tracks must be greater or equal to 1."));
	}

	if (mDistanceGate < 0) {
		AddError(wxT("The minimal distance (DistanceGate) cannot be smaller than zero."));
	}


	for (int i = 0;i < mMaxNumber;i++)      // initiate mMaxNumber Track classes
	{
		sharedage.push_back(0);
		oldshared.push_back(0);
		restingtraj.push_back(0);
		mTracks[i] = Track(i);
		mTracks[i].AddPoint(cvPoint2D32f(320, 240),
				       mCore->mDataStructureInput.mFrameNumber);

	}
}

void THISCLASS::OnReloadConfiguration() {
}
void THISCLASS::DataAssociation()
{

	int id;
	competitors.clear(); // erase the list of competitors;
	for (id = 0;id < mMaxNumber;id++)   // loop trough all trajectories
	{
		double min_dist = 1000000;
		DataStructureParticles::tParticleVector::iterator min_dist_id;


		for (DataStructureParticles::tParticleVector::iterator p = particles->begin(); // find the "closest" particle for object 'id'
		        p != particles->end();
		        p++){
			double dist = GetCost(id, p->mCenter);
			//	printf("%f %f %f\n",p->mCenter.x,p->mCenter.y,dist);
			if (dist < min_dist)
			{
				min_dist = dist;
				min_dist_id = p; // save pointer to particle
			}
		}
		// the closest particle 'min_dist_id' has distance 'min_dist' now

		if (sqrt(min_dist) < mDistanceGate){ // if good enough (threshold) take it, otherwise reject
			if (min_dist_id->mID == -1){ // if particle is not associated yet
				min_dist_id->mID = id; // associate best particle with object id

				AddPoint(id, min_dist_id->mCenter);
			}
			else{  // otherwise just take this point and add id to the list of competitors, but only if this behavior is desired
				if (mShareTrajectories){
					AddPoint(id, min_dist_id->mCenter);
					AddCompetitor(id);
				}
			}
		}
	} // end for each id 1

	// At this point, two or more trajectories can share one particle. However, all
	// trajectories that join an already associated particle are kept as competitors

	/*

		// Now, check which particles have not been associated
		int fp,ap;		// free particles, associated particles

		FindFreeParticles(&fp,&ap);

		if(fp && ap <mMaxNumber){ // if there are unassociated particles and less associated particles than objects
			if(!competitors.empty()){
				//CleanParticles(); // delete all assigned particles from the particle list
				AssociateParticlesToCompetitors(mDistanceGate);
			}  // end if !competitors.empty
		} // end if fp && ap<num_objects
	*/
	avg_speed = mDistanceGate / 2;

	///////////////////// Determine number and age of shared trajectories //////////////////

	std::vector<int> shared;
	int nrshared = CountSharedTrajectories(&shared);

	int maxageexceeded = -1;

	if (nrshared == 0){
		for (int i = 0; i < mMaxNumber; i++) sharedage.at(i) = 0;
	}
	else{
		for (int i = 0; i < mMaxNumber; i++){
			if (shared.at(i)){
				sharedage.at(i)++;
				if (GetCost(i, *GetCritPoint(i)) > avg_speed*avg_speed) maxageexceeded = i;
			}
			else
				sharedage.at(i) = 0;
			SetCritPoint(i); // remember the actual point as 'last-known-good'
		}
	}


	///////////////////////////// Create new trajectories for noise  //////////////////////

	//CleanParticles();
	//	printf("We have now %d particles to assign, and have already %d noise trajectories\n",particles->size(),ptargets.size());
	DataStructureParticles::tParticleVector::iterator p;
	for (p = particles->begin();p != particles->end();p++) // assign particles to existing noise trajectories
	{
		double min_dist = 1000000;
		std::map<int, Track>::iterator min_dist_id = ptargets.begin();


		for (std::map<int, Track>::iterator t = ptargets.begin(); t != ptargets.end(); t++)
		{
			double dist = GetDist(&(t->second.trajectory.back()), &(p->mCenter));
			//		printf("%0.2f %0.2f %0.2f %0.2f %0.2f\n",p->mCenter.x,p->mCenter.y,t->trajectory.back().x,t->trajectory.back().y,dist);
			if (dist < min_dist)
			{
				min_dist = dist;
				min_dist_id = t; // save pointer to closest track
			}
		}

		// the closest track 'min_dist_id' has distance 'min_dist' now

		if (sqrt(min_dist) < mDistanceGate && p->mID == -1){ // if good enough (threshold) take it, otherwise reject
			p->mID = min_dist_id->first; // associate best particle with object id
			min_dist_id->second.AddPoint(p->mCenter, mCore->mDataStructureInput.mFrameNumber);
		}
		else{
			if (min_dist_id != ptargets.begin()) ptargets.erase(min_dist_id); // erase noise trajectories that did not find anyone
		}
	} // end for each id 1

	id = ptargets.size();
	for (p = particles->begin();p != particles->end();p++){ // create new noise trajectories
		if (p->mID == -1){
			//			printf("Create new noise trajectory (%d)\n",id);
			//Track* tmpTrack = new Track(id,trackingimg,mMaxNumber);
			ptargets[id] = Track(id);
			//delete tmpTrack;
			ptargets[id].AddPoint(p->mCenter, mCore->mDataStructureInput.mFrameNumber);
			p->mID = id;
			id++;
		}
	}

	int found = 0;
	while (!found){ // this loop goes until all particles/trajectories are known
		found = 1;
		for (std::map<int, Track>::iterator t = ptargets.begin(); t != ptargets.end(); t++){ // delete noise trajectories that are not assigned
			found = 0;
			for (p = particles->begin();p != particles->end();p++) if (p->mID == t->first) found = 1;
			if (!found){
				ptargets.erase(t);
				break;
			}
		}
	}

	///////////// If a noise trajectory seems to be reasonable, swap with the closest shared trajectory ///////////////////

	found = 0;
	for (std::map<int, Track>::iterator t = ptargets.begin(); t != ptargets.end(); t++){
		double dist = GetDist(&t->second.trajectory.front(), &t->second.trajectory.back());
		if (dist > avg_speed * avg_speed && t->second.trajectory.size() > 5){
			//			printf("Found promising noise trajectory (%0.2f)\n",sqrt(dist));
			// A promising trajectory must have a certain length + certain age to avoid to track on noise bursts
			double min_dist = 1000000;
			int min_dist_id = -1;

			for (id = 0; id < mMaxNumber; id++){
				if (shared.at(id)){
					double dist = GetDist(GetPos(id), &t->second.trajectory.back());
					if (dist < min_dist){
						min_dist = dist;
						min_dist_id = id;
					}
				}
				if (min_dist_id != -1 && dist < (avg_speed + mDistanceGate)*(avg_speed + mDistanceGate)){
					//printf("Found candidate to swap with!\n");
					shared.at(min_dist_id) = 0;
					mTracks[min_dist_id].trajectory.pop_back();
					AddPoint(min_dist_id, t->second.trajectory.back());
					ptargets.erase(t);
					found = 1; // do only one at a time
					break;
				}
			}
		}
		if (found) break;
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
double THISCLASS::GetCost(int id, CvPoint2D32f p){

	return(
	          (mTracks[id].trajectory.back().x -p.x)*
	          (mTracks[id].trajectory.back().x - p.x)
	          +
	          (mTracks[id].trajectory.back().y - p.y)*
	          (mTracks[id].trajectory.back().y - p.y)
	      );
}

void THISCLASS::OnStep() {
	// Particles we want to associate
	particles = mCore->mDataStructureParticles.mParticles;

	DataAssociation();	// associate all points with their nearest neighbor

	// Set these particles
	mCore->mDataStructureTracks.mTracks = &mTracks;

	// Let the DisplayImage know about our image
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) {
		//de.SetParticles(particles);
		de.SetMainImage(mCore->mDataStructureInput.mImage);
		de.SetTrajectories(true);
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


void THISCLASS::AddCompetitor(int c){
	int exist = 0;
	for (std::vector<int>::iterator it = competitors.begin(); it != competitors.end(); it++)
		if (*it == c)
			exist = 1;
	if (!exist)
		competitors.push_back(c);
}

/** \brief Checks how many particles have been associated or are free respectively
* \param fp : number of free particles
* \param ap : number of associated particles
*/
void THISCLASS::FindFreeParticles(int* fp, int* ap)
{
	*fp = 0;
	*ap = 0;
	for (DataStructureParticles::tParticleVector::iterator p = particles->begin();
	        p != particles->end();
	        p++)
		if (p->mID == -1)
			(*fp)++;
		else
			(*ap)++;
}

/** Deletes all used (associated) particles from the list of particles */
void THISCLASS::CleanParticles()
{
	DataStructureParticles::tParticleVector temp;
	for (unsigned int i = 0; i < particles->size(); i++) temp.push_back(particles->at(i));

	particles->clear();

	for (DataStructureParticles::tParticleVector::iterator p = temp.begin();
	        p != temp.end();
	        p++){
		if (p->mID == -1)
			particles->push_back(*p);
	}
}

void THISCLASS::AssociateParticlesToCompetitors(int max_speed){
	for (DataStructureParticles::tParticleVector::iterator p = particles->begin();
	        p != particles->end();
	        p++){
		const double INFTY = 1000000000;
		double min_dist = (double) INFTY;
		bool good_competitor_found = false; // True iff a reasonable competitor has been found
		std::vector<int>::iterator min_dist_id = competitors.begin();

		for (std::vector<int>::iterator it = competitors.begin(); it != competitors.end(); it++){
			double dist = GetCost(*it, p->mCenter);
			if (dist < min_dist)
			{
				min_dist = dist;
				min_dist_id = it; // save pointer to competitor
			}

		}


		if (sqrt(min_dist) < max_speed)
		{ // take a particle that is not to far away
			mTracks[*min_dist_id].trajectory.pop_back(); // Remove the oldest trajectory point of the trajectory history
			AddPoint(*min_dist_id, p->mCenter); // Add the current point to the trajectory history
			good_competitor_found = true; // In this case say we have found the good competitor
		}


		// [2006-10-19-14-08, Clement Hongler:] commented this line because allows potential trajectories overlap
		// if(min_dist_id!=competitors.begin()) competitors.erase(min_dist_id); // ...and delete it
		// Replaced with this one that should do what we expect

		// Iff we have found a good competitor (it has already been added to the particle history
		if (good_competitor_found == true) {
			competitors.erase(min_dist_id); // We can remove it from the list of the competitors
		}

	} // end for every particle
}


int THISCLASS::CountSharedTrajectories(std::vector<int>* shared)
{
	std::vector<CvPoint2D32f> locs;
	std::vector<CvPoint2D32f> redundant;


	int sht = 0;

	for (int i = 0;i < mMaxNumber; i++){
		CvPoint2D32f* act = GetPos(i);
		int found = 0;

		for (unsigned int j = 0; j < locs.size(); j++){
			if (act->x == locs.at(j).x && act->y == locs.at(j).y){
				int isredundant = 0;
				for (unsigned int k = 0; k < redundant.size(); k++){
					if (act->x == redundant.at(k).x && act->y == redundant.at(k).y) isredundant = 1;
				}
				if (!isredundant) sht++;
				found = 1;
			}
		}
		if (!found){
			locs.push_back(*act);
			shared->push_back(0);
		}
		else{
			shared->push_back(sht);
			redundant.push_back(*act);
		}
	}

	//	for(i=0;i<mMaxNumber; i++) printf("%d",shared->at(i));
	//	printf("\n");

	return(sht);
}

void THISCLASS::SetCritPoint(int id)
{
	mTracks[id].SetCritPoint(&mTracks[id].trajectory.back());
}


CvPoint2D32f* THISCLASS::GetCritPoint(int id)
{
	return(&mTracks[id].critpoint);
}

double THISCLASS::GetDist(CvPoint2D32f *p1, CvPoint2D32f *p2)
{
	return(
	          (p1->x - p2->x)*
	          (p1->x - p2->x)
	          +
	          (p1->y - p2->y)*
	          (p1->y - p2->y)
	      );
}

/** Yields current position of the object specified by id.
*
* \param id : id of the object which position will be returned.
* \return Pixel coordinate of object specified by id (subpixel accuracy)
*/
CvPoint2D32f* THISCLASS::GetPos(int id)
{
	//printf("%f %f\n",mTracks[0].trajectory.at(0).x,mTracks.at(0).trajectory.at(0).y);
	//	printf("Size %d\n",mTracks.at(id).trajectory.size());
	return(&mTracks[id].trajectory.back());
}

void THISCLASS::OnStepCleanup() {
	mCore->mDataStructureParticles.mParticles = 0;
}

void THISCLASS::OnStop() {
}
