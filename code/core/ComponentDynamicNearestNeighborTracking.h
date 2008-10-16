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
	/**
	 * Filter tracks we aren't using any more. 
	 */
	void FilterTracks();
	void DataAssociation();
	void ClearDistanceArray();
	void InitializeTracks();
	double GetCost(const Track & track, CvPoint2D32f p);
	void AddPoint(int i, CvPoint2D32f p);

	DataStructureParticles::tParticleVector *particles;
	std::map<int, double*> distanceArray;
	unsigned int maxParticles;
	int mNextTrackId;

	// Parameters
	double mMaxDistance; //!< (configuration) The maximum distance between a track and a particle to attach
	double mMinNewTrackDistance; //!< (configuration) The minimum distance between new tracks
	int mFrameKillThreshold; //!< (configuration) The number of frames a point doesn't get updated before it gets destroyed. 
	DataStructureTracks::tTrackMap mTracks;
	Display mDisplayOutput;									//!< The Display showing the last acquired image and the particles.

};

#endif

