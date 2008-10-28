#ifndef HEADER_ComponentKalmanFilterTrack
#define HEADER_ComponentKalmanFilterTrack

#include <cv.h>
using namespace std;
#include <vector>

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
	Track& GetOrMakeTrack(int id, CvPoint2D32f p);
	void EraseTrack(int id);
	CvPoint2D32f StepFilter(int trackID, CvPoint2D32f newMeasurement);
	DataStructureParticles::tParticleVector mParticles;
	map<int, CvKalman *> mKalman;
	map<int, CvMat *> mX_k; // state.  position, velocity
	map<int, CvMat *> mZ_k; // measurement.  position (x,y)
	map<int, CvMat *> mF;
	DataStructureTracks::tTrackMap mOutputTracks;

	wxDateTime mLastTs;

	unsigned int mWindowSize; //!< (configuration) The maximum number of objects that are to track.

	
	Display mDisplayOutput;	//!< The Display showing the last acquired image and the particles.
};

#endif

