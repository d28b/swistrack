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
	Component *Create() {return new ComponentTracking(mCore);}

private:
	std::vector<int> oldshared;
	std::vector<int> sharedage;
	std::vector<int> restingtraj;
	int mMaxNumber;		//!< (configuration) The maximum number of objects that are to track.
	double mDistanceGate; //!< (configuration) The maximum distance a particle can move during one step.
	DataStructureTracks::tTrackVector mTracks;
	Display mDisplayOutput;									//!< The Display showing the last acquired image and the particles.	
};

#endif

