#ifndef HEADER_ComponentOutputParticles
#define HEADER_ComponentOutputParticles

#include "Component.h"

//! A component that writes particles to the communication interface. This information is usually transmitted to clients that connect via TCP to SwisTrack.
class ComponentOutputParticles: public Component {

public:
	//! Constructor.
	ComponentOutputParticles(SwisTrackCore *stc);
	//! Destructor.
	~ComponentOutputParticles();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {
		return new ComponentOutputParticles(mCore);
	}

private:
};

#endif
