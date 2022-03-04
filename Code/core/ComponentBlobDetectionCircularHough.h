#ifndef HEADER_ComponentBlobDetectionCircularHough
#define HEADER_ComponentBlobDetectionCircularHough

#include <opencv2/core.hpp>
#include "Component.h"
#include "DataStructureParticles.h"

//! A component that detects blobs that have a certain size and stores them as particles in DataStructureParticle.
class ComponentBlobDetectionCircularHough: public Component {

public:
	//! Constructor.
	ComponentBlobDetectionCircularHough(SwisTrackCore * stc);
	//! Destructor.
	~ComponentBlobDetectionCircularHough();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();

	Component * Create() {
		return new ComponentBlobDetectionCircularHough(mCore);
	}

private:
	int AccumulatorResolution;			//!< (configuration) Acumulator resolution for Circular Hough Transform.
	int MinDist;						//!< (configuration) The minimum distance (in pixels) between two circle centers.
	int EdgeDetectionThreshold;			//!< (configuration) Canny Edge Detection threshold.
	int CenterDetectionThreshold;		//!< (configuration) Circular Edge Detection threshold.
	bool SelectionBySize; 				//!< (configuration) If the selection is based on the circle size
	int MinRadius;						//!< (configuration) The minimum radius (in pixels) to threshold detected circle
	int MaxRadius;						//!< (configuration) The maximum radius (in pixels) to threshold detected circle.
	bool LimitNumberOfCircles;			//!< (configuration) Limiting the number of detected circles
	int MaxNumberOfCircles;				//!< (configuration) The maximum number of circles that are to be detected.

	DataStructureParticles::tParticleVector mParticles;	//!< The list of detected particles. This is the list given to DataStructureParticles.
	Display mDisplayOutput;				//!< The Display showing the last acquired image and the particles.
};

#endif

