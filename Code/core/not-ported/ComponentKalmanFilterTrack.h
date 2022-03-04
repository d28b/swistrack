#ifndef HEADER_ComponentKalmanFilterTrack
#define HEADER_ComponentKalmanFilterTrack

#include <opencv2/core.hpp>
#include <vector>

#include "Component.h"
#include "DataStructureParticles.h"

//! A component that detects blobs that have a certain size and stores them as particles in DataStructureParticle.
class ComponentKalmanFilterTrack: public Component {

 public:
	//! Constructor.
	ComponentKalmanFilterTrack(SwisTrackCore * stc);
	//! Destructor.
	~ComponentKalmanFilterTrack();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();

	Component * Create() {
		return new ComponentKalmanFilterTrack(mCore);
	}

private:
	Track& GetOrMakeTrack(int id, cv::Point2f p);
	void EraseTrack(int id);
	cv::Point2f StepFilter(int trackID, cv::Point2f newMeasurement, cv::Point2f * lastMeasurement);
	DataStructureParticles::tParticleVector mParticles;
	std::map<int, CvKalman *> mKalman;
	std::map<int, cv::Mat> mZ_k; // measurement.  position (x,y)
	std::map<int, cv::Mat> mF;
	DataStructureTracks::tTrackMap mOutputTracks;

	wxDateTime mLastTs;
	unsigned int mWindowSize; //!< (configuration) The maximum number of objects that are to track.

	Display mDisplayOutput;	//!< The Display showing the last acquired image and the particles.
};

#endif

