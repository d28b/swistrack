#ifndef HEADER_ComponentNearestNeighborTracking
#define HEADER_ComponentNearestNeighborTracking

#include <cv.h>
#include "Component.h"
#include "DataStructureParticles.h"

//! A component that detects blobs that have a certain size and stores them as particles in DataStructureParticle.
class ComponentNearestNeighborTracking: public Component {

public:
	//! Constructor.
	ComponentNearestNeighborTracking(SwisTrackCore *stc);
	//! Destructor.
	~ComponentNearestNeighborTracking();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {return new ComponentNearestNeighborTracking(mCore);}

private:
	void DataAssociation();
	double GetCost(int id,CvPoint2D32f p);
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

