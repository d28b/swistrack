#ifndef HEADER_ComponentOutputParticles
#define HEADER_ComponentOutputParticles

#include "Component.h"

class ComponentOutputParticles: public Component {

public:
	ComponentOutputParticles(SwisTrackCore *stc);
	~ComponentOutputParticles();

	// Overwritten Component methods
	void OnStart();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {return new ComponentOutputParticles(mCore);}

private:
};

#endif

