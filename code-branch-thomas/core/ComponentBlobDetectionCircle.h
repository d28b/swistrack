#ifndef HEADER_ComponentBlobDetectionCircle
#define HEADER_ComponentBlobDetectionCircle

#include "Component.h"

class ComponentBlobDetectionCircle: public Component {

public:
	ComponentBlobDetectionCircle(SwisTrackCore *stc);
	~ComponentBlobDetectionCircle();

	// Overwritten Component methods
	void OnStart();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {return new ComponentBlobDetectionCircle(mCore);}

private:
	DataStructureParticles::tParticleVector mParticles;
};

#endif

