#ifndef HEADER_ComponentBlobDetectionCircularHough
#define HEADER_ComponentBlobDetectionCircularHough

#include <cv.h>
#include "Component.h"
#include "DataStructureParticles.h"

//! A component that detects blobs that have a certain size and stores them as particles in DataStructureParticle.
class ComponentBlobDetectionCircularHough: public Component {

public:
	//! Constructor.
	ComponentBlobDetectionCircularHough(SwisTrackCore *stc);
	//! Destructor.
	~ComponentBlobDetectionCircularHough();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	int mChannelMain;					//!< The channel to subtract from.
	int mChannelSubtract;					//!< The channel to subtract.
	bool SelectionBySize; 					//!< (configuration) If the selection is based on the circle size
	bool LimitNumberofCircles;				//!< (configuration) Limiting the number of detected circles
	int MaxNumberofCircles;					//!< (configuration) The maximum number of circles that are to be detected.
	int MinDist;						//!< (configuration) The minimum distance (in pixels) between two circle centers.
	int MinRadius;						//!< (configuration) The minimum radius (in pixels) to threshold detected circle
	int MaxRadius;						//!< (configuration) The maximum radius (in pixels) to threshold detected circle.
	int dp;							//!< (configuration) Acumulator resolution for Circular Hough Transform.
	int Param1;						//!< (configuration) Canny Edge Detection threshold.
	int Param2;						//!< (configuration) Circular Edge Detection threshold.	
	DataStructureParticles::tParticleVector mParticles;	//!< The list of detected particles. This is the list given to DataStructureParticles.
	Display mDisplayOutput;					//!< The Display showing the last acquired image and the particles.
	Component *Create() {
		return new ComponentBlobDetectionCircularHough(mCore);
	}
	
};

#endif

