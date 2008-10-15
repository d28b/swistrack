#ifndef HEADER_ComponentDynamicNearestNeighborTracking
#define HEADER_ComponentDynamicNearestNeighborTracking

#include <cv.h>
#include "Component.h"
#include "DataStructureParticles.h"

//! A basic tracking component that takes pre-filtered particales and
// adds them to the nearest track.
class ComponentDynamicNearestNeighborTracking: public Component {

public:
	//! Constructor.
	ComponentDynamicNearestNeighborTracking(SwisTrackCore *stc);
	//! Destructor.
	~ComponentDynamicNearestNeighborTracking();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {
		return new ComponentDynamicNearestNeighborTracking(mCore);
	}

private:
	void DataAssociation();
	double GetCost(int id, CvPoint2D32f p);
	void AddPoint(int i, CvPoint2D32f p);

	DataStructureParticles::tParticleVector *particles;
	double** distanceArray;
	int maxParticles;

	// Parameters
	double mMaxDistance; //!< (configuration) The maximum distance between a track and a particle to attach
	int mMaxNumber;		//!< (configuration) The maximum number of objects that are to track.
	DataStructureTracks::tTrackVector mTracks;
	Display mDisplayOutput;									//!< The Display showing the last acquired image and the particles.
};

#endif

