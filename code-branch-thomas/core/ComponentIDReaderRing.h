#ifndef HEADER_ComponentIDReaderRing
#define HEADER_ComponentIDReaderRing

#include "cv.h"
#include "Component.h"
#include "DataStructureParticles.h"
#include "DisplayImageParticles.h"

//! A component that detects blobs that have a certain size and stores them as particles in DataStructureParticle.
class ComponentIDReaderRing: public Component {

public:
	typedef std::list<int> tCodeList;

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
	Component *Create() {return new ComponentIDReaderRing(mCore);}

private:
	double mRingRadiusInner;	//!< (configuration) The inner radius of the ring.
	double mRingRadiusOuter;	//!< (configuration) The outer radius of the ring.
	int mChips;					//!< (configuration) The number of chips.
	tCodeList mCodes;			//!< (configuration) The maximum number of blobs that are to detect.

	DisplayImageParticles mDisplayImageOutput;				//!< The DisplayImage showing the last acquired image and the particles.

};

#endif

