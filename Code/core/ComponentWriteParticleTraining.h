#ifndef HEADER_ComponentWriteParticleTraining
#define HEADER_ComponentWriteParticleTraining

#include "Component.h"

//! A component that writes particles to the communication interface. This information is usually transmitted to clients that connect via TCP to SwisTrack.
class ComponentWriteParticleTraining: public Component {

public:
	//! Constructor.
	ComponentWriteParticleTraining(SwisTrackCore *stc);
	//! Destructor.
	~ComponentWriteParticleTraining();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {
		return new ComponentWriteParticleTraining(mCore);
	}

private:
	wxString mFileName;	
};

#endif
