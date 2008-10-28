#ifndef HEADER_ComponentKalmanFilterTrack
#define HEADER_ComponentKalmanFilterTrack

#include <cv.h>
#include "Component.h"
#include "DataStructureParticles.h"

//! A component that detects blobs that have a certain size and stores them as particles in DataStructureParticle.
class ComponentKalmanFilterTrack: public Component {

 public:
	//! Constructor.
	ComponentKalmanFilterTrack(SwisTrackCore *stc);
	//! Destructor.
	~ComponentKalmanFilterTrack();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {
		return new ComponentKalmanFilterTrack(mCore);
	}

private:
	Track& WindowForTrack(int id);
	DataStructureParticles::tParticleVector mParticles;
	double** distanceArray;
	int maxParticles;

	// Parameters
	unsigned int mWindowSize;		//!< (configuration) The maximum number of objects that are to track.
	//	DataStructureTracks::tTrackVector mInputTracks;
	DataStructureTracks::tTrackMap mOutputTracks;
	DataStructureTracks::tTrackMap mWindows;
	
	Display mDisplayOutput;									//!< The Display showing the last acquired image and the particles.
};

#endif

