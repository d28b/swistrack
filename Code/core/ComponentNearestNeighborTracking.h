#ifndef HEADER_ComponentNearestNeighborTracking
#define HEADER_ComponentNearestNeighborTracking

#include <opencv2/opencv.hpp>
#include "Component.h"
#include "DataStructureParticles.h"

//! A basic tracking component that adds particles to the nearest track.
class ComponentNearestNeighborTracking: public Component {

public:
	//! Constructor.
	ComponentNearestNeighborTracking(SwisTrackCore * stc);
	//! Destructor.
	~ComponentNearestNeighborTracking();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();

	Component * Create() {
		return new ComponentNearestNeighborTracking(mCore);
	}

private:
	double mMaxDistance2; 						//!< (configuration) The maximum distance between a track and a particle to attach.
	int mMaxLostTrackFrames;					//!< (configuration) The number of frames to ignore (gaps to accept) until a track is declared lost.
	unsigned int mMaxNumberOfTracks;			//!< (configuration) The maximum number of objects to track.
	unsigned int mMaxNumberOfPoints;			//!< (configuration) The maximum number of points per track to keep in memory.

	DataStructureTracks::tTrackMap mTracks;		//!< The tracks.
	int mNextTrackID;							//!< The next unused track ID.
	Display mDisplayOutput;						//!< The Display showing the last acquired image and the particles.
};

#endif

