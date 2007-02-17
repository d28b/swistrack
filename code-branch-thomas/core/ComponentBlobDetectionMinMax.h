#ifndef HEADER_ComponentBlobDetectionMinMax
#define HEADER_ComponentBlobDetectionMinMax

#include "cv.h"
#include "Component.h"
#include "DataStructureParticles.h"

//! A component that detects blobs that have a certain size and stores them as particles in DataStructureParticle.
class ComponentBlobDetectionMinMax: public Component {

public:
	//! Constructor.
	ComponentBlobDetectionMinMax(SwisTrackCore *stc);
	//! Destructor.
	~ComponentBlobDetectionMinMax();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {return new ComponentBlobDetectionMinMax(mCore);}

private:
	double mMinArea;	//!< (configuration) The minimum area of a blob.
	double mMaxArea;	//!< (configuration) The maximum area of a blob.
	int mMaxNumber;		//!< (configuration) The maximum number of blobs that are to detect.

	int mFirstDilate;	//!< The number of dilation operations to apply to the binary image.
	int mFirstErode;	//!< The number of erode operations to apply to the binary image.
	int mSecondDilate;	//!< The number of second dilation operations to apply to the binary image.

	DataStructureParticles::tParticleVector mParticles;		//!< The list of detected particles. This is the list given to DataStructureParticles.

	//! Returns the compactness of a contour. Used by the blob detection algorithm.
	double GetContourCompactness(const void* contour);
};

#endif

