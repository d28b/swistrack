#ifndef HEADER_ComponentNearestNeighborTracking
#define HEADER_ComponentNearestNeighborTracking

#include <opencv2/opencv.hpp>
#include "Component.h"
#include "DataStructureParticles.h"

//! A basic tracking component that takes pre-filtered particales and
// adds them to the nearest track.
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
	void DataAssociation();
	double GetCost(int id, cv::Point2f p);
	void AddPoint(int i, cv::Point2f p);

	DataStructureParticles::tParticleVector * particles;
	double** distanceArray;
	int maxParticles;

	// Parameters
	double mMaxDistance; //!< (configuration) The maximum distance between a track and a particle to attach
	int mMaxNumber;		//!< (configuration) The maximum number of objects that are to track.
	DataStructureTracks::tTrackMap mTracks;
	Display mDisplayOutput;									//!< The Display showing the last acquired image and the particles.
};

#endif

