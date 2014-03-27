#ifndef HEADER_ComponentTrackSmoothing
#define HEADER_ComponentTrackSmoothing

#include <cv.h>
#include "Component.h"
#include "DataStructureParticles.h"

//! A component that detects blobs that have a certain size and stores them as particles in DataStructureParticle.
class ComponentTrackSmoothing: public Component {

 public:
	//! Constructor.
	ComponentTrackSmoothing(SwisTrackCore *stc);
	//! Destructor.
	~ComponentTrackSmoothing();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {
		return new ComponentTrackSmoothing(mCore);
	}

private:
	Track& WindowForTrack(int id);
	DataStructureParticles::tParticleVector mParticles;

	// Parameters
	unsigned int mWindowSize;		//!< (configuration) The maximum number of objects that are to track.
	//	DataStructureTracks::tTrackVector mInputTracks;
	DataStructureTracks::tTrackMap mOutputTracks;
	DataStructureTracks::tTrackMap mWindows;
	
	Display mDisplayOutput;									//!< The Display showing the last acquired image and the particles.
};

#endif

