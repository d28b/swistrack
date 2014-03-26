#ifndef HEADER_ComponentBlobDetectionMinMax
#define HEADER_ComponentBlobDetectionMinMax

#include <cv.h>
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
	Component *Create() {
		return new ComponentBlobDetectionMinMax(mCore);
	}

private:
	int mMinArea;	//!< (configuration) The minimum area of a blob.
	int mMaxArea;	//!< (configuration) The maximum area of a blob.
	unsigned int mMaxNumber;	//!< (configuration) The maximum number of blobs that are to detect.
	bool mAreaSelection; //!< (configuration) If the selection is based on the blob area
	double mMinCompactness;	//!< (configuration) The minimum Compactness of a blob.
	double mMaxCompactness;	//!< (configuration) The maximum Compactness of a blob.
	bool mCompactnessSelection; //!< (configuration) If the selection is based on the blob Compactness
	double mMinOrientation;	//!< (configuration) The minimum Compactness of a blob.
	double mMaxOrientation;	//!< (configuration) The maximum Compactness of a blob.
	bool mOrientationSelection; //!< (configuration) If the selection is based on the blob Orientation
	DataStructureParticles::tParticleVector mParticles;		//!< The list of detected particles. This is the list given to DataStructureParticles.
	Display mDisplayOutput;									//!< The Display showing the last acquired image and the particles.

	//! Returns the compactness of a contour. Used by the blob detection algorithm.
	double GetContourCompactness(const void* contour);
};

#endif

