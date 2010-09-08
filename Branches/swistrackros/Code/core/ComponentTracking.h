#ifndef HEADER_ComponentTracking
#define HEADER_ComponentTracking

#include <cv.h>
#include "Component.h"
#include "DataStructureParticles.h"

//! A component that detects blobs that have a certain size and stores them as particles in DataStructureParticle.
class ComponentTracking: public Component {

public:
	//! Constructor.
	ComponentTracking(SwisTrackCore *stc);
	//! Destructor.
	~ComponentTracking();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {
		return new ComponentTracking(mCore);
	}

private:
	void DataAssociation();
	double GetCost(int id, CvPoint2D32f p);
	void AddPoint(int i, CvPoint2D32f p);
	void AddCompetitor(int c);
	void FindFreeParticles(int* fp, int* ap);
	void CleanParticles();
	void AssociateParticlesToCompetitors(int max_speed);
	int CountSharedTrajectories(std::vector<int>* shared);
	void SetCritPoint(int id);
	CvPoint2D32f* GetCritPoint(int id);
	double GetDist(CvPoint2D32f *p1, CvPoint2D32f *p2);
	CvPoint2D32f* GetPos(int id);

	DataStructureParticles::tParticleVector *particles;

	/** Vector containing the id number of objects competing for tracks */
	std::vector<int> competitors;
	/** Potential trajectories */
	DataStructureTracks::tTrackMap ptargets;

	std::vector<int> oldshared;
	std::vector<int> sharedage;
	std::vector<int> restingtraj;
	/** 'Average' speed of objects to track */
	int avg_speed;


	// Parameters

	int mShareTrajectories; //!< If set to one, trajectories can share a particle, otherwise not (suitable for marker based tracking)
	int mMaxNumber;		//!< (configuration) The maximum number of objects that are to track.
	double mDistanceGate; //!< (configuration) The maximum distance a particle can move during one step.
	DataStructureTracks::tTrackMap mTracks;
	Display mDisplayOutput;									//!< The Display showing the last acquired image and the particles.
};

#endif

