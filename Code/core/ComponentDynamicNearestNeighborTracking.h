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
	void DataAssociation(DataStructureParticles::tParticleVector * particles);
	void ClearDistanceArray();
	void ClearTracks();
	double GetCost(const Track & track, CvPoint2D32f p);
	void AddParticle(int i, Particle * p);
	bool ColorsMatch(const Track & track, const Particle & particle);


	std::map<int, double*> squareDistanceArray;
	unsigned int maxParticles;
	int mNextTrackId;

	// Parameters
	double mColorSimilarityThreshold; 
	double mMaxDistanceSquared; 
	double mMinNewTrackDistanceSquared; 
	int mFrameKillThreshold; 
	int mTrackDistanceKillThresholdSquared; 
	DataStructureTracks::tTrackMap mTracks;
	Display mDisplayOutput;	//!< The Display showing the last acquired image and the particles.

	CvHistogram * mTmp1, * mTmp2;

};

#endif

