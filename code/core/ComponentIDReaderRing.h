#ifndef HEADER_ComponentIDReaderRing
#define HEADER_ComponentIDReaderRing

#include <cv.h>
#include "Component.h"
#include "DataStructureParticles.h"
#include "ObjectList.h"

//! A component that detects blobs that have a certain size and stores them as particles in DataStructureParticle.
class ComponentIDReaderRing: public Component {

public:
	//! Constructor.
	ComponentIDReaderRing(SwisTrackCore *stc);
	//! Destructor.
	~ComponentIDReaderRing();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {
		return new ComponentIDReaderRing(mCore);
	}

private:
	float mRingRadiusInner;	//!< (configuration) The squared inner radius of the ring.
	float mRingRadiusOuter;	//!< (configuration) The squared outer radius of the ring.
	ObjectList *mObjectList;	//!< The object list.

	// Precalculated values
	float mRingRadiusInner2;	//!< The squared inner radius of the ring.
	float mRingRadiusOuter2;	//!< The squared outer radius of the ring.
	int mRingValuesMax;			//!< The allocated number of values for the two arrays mAngles and mValues.
	int mRingCount;				//!< Counts how many ring values have been filled in.
	float *mRingAngles;			//!< The angles of all pixel on the ring.
	int *mRingValues;			//!< The values of all pixel on the ring.
	int mCodeLength;			//!< The length of the code.
	float *mBinValues;			//!< A bin for each chip of the code.
	int *mBinCounts;			//!< A counter for each bin.

	CvPoint mOffset;			//!< The offset from the blob center to the pattern origin.
	CvSize mSize;				//!< The size of the pattern.
	float *mDistances;			//!< The distances of each pixel.

	Display mDisplayOutput;									//!< The Display showing the last acquired image and the particles.

	// Adds all rings values to the corresponding bins.
	void RingToBins(float shift);
};

#endif

