#ifndef HEADER_ComponentOutputParticles
#define HEADER_ComponentOutputParticles

#include "Component.h"

class ComponentOutputParticles: public Component {

public:
	//! Constructor.
	ComponentOutputParticles(SwisTrackCore * stc);
	//! Destructor.
	~ComponentOutputParticles();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();

	Component * Create() {
		return new ComponentOutputParticles(mCore);
	}

private:
};

#endif
