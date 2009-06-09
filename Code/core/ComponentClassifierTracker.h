#ifndef HEADER_ComponentClassifierTracker
#define HEADER_ComponentClassifierTracker

#include <cv.h>
#include <set>
#include "Component.h"

#include "DataStructureParticles.h"

#include "DataAssociationClassifier.h"

typedef map<int, vector<Particle > > ParticleCache;

//! A basic tracking component that takes pre-filtered particales and
// adds them to the nearest track.
class ComponentClassifierTracker: public Component {

public:
	//! Constructor.
	ComponentClassifierTracker(SwisTrackCore *stc);
	//! Destructor.
	~ComponentClassifierTracker();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {
		return new ComponentClassifierTracker(mCore);
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
	void EraseTracks(set<int> trackIds);
	void EraseTrack(int id);


	std::map<int, double*> squareDistanceArray;
	unsigned int maxParticles;
	int mNextTrackId;
	ParticleCache mParticleCache;
	DataStructureTracks::tTrackMap mTracks;

	// Parameters
	double mColorSimilarityThreshold; 
	double mMaxDistanceSquared; 
	double mMinNewTrackDistanceSquared; 
	int mFrameKillThreshold; 
	int mTrackDistanceKillThresholdSquared; 

	
	DataAssociationClassifier mClassifier;

	Display mDisplayOutput;	//!< The Display showing the last acquired image and the particles.

};

#endif

