#ifndef HEADER_ComponentBlobDetectionCircle
#define HEADER_ComponentBlobDetectionCircle

#include "Component.h"

//! A component that detect circular blobs.
class ComponentBlobDetectionCircle: public Component {

public:
	//! Constructor.
	ComponentBlobDetectionCircle(SwisTrackCore *stc);
	//! Destructor.
	~ComponentBlobDetectionCircle();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {return new ComponentBlobDetectionCircle(mCore);}

private:
	DataStructureParticles::tParticleVector mParticles;		//!< The list of detected particles. This is the list given to DataStructureParticles.
};

#endif

