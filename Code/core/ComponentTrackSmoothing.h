#ifndef HEADER_ComponentTrackSmoothing
#define HEADER_ComponentTrackSmoothing

#include <opencv2/opencv.hpp>
#include "Component.h"
#include "DataStructureParticles.h"

//! A component that detects blobs that have a certain size and stores them as particles in DataStructureParticle.
class ComponentTrackSmoothing: public Component {

public:
	//! Constructor.
	ComponentTrackSmoothing(SwisTrackCore * stc);
	//! Destructor.
	~ComponentTrackSmoothing();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();

	Component * Create() {
		return new ComponentTrackSmoothing(mCore);
	}

private:
	class State {
	public:
		bool seen;
		int gaps;
		std::deque<TrackPoint> points;
		Track * track;

		State();
		void AddPoint(float x, float y, int frameNumber, unsigned int windowSize);
	};

	unsigned int mWindowSize;					//!< (configuration) The window size.
	std::map<int, State> mState;				//!< The state of each track.
	DataStructureTracks::tTrackMap mTracks;		//!< The smoothed tracks.
	Display mDisplayOutput;						//!< The Display showing the last acquired image and the particles.
};

#endif

