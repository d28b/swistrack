#ifndef HEADER_ComponentOutputDataAssociationTraining
#define HEADER_ComponentOutputDataAssociationTraining

#include "Component.h"

//! A component that writes particles to the communication interface. This information is usually transmitted to clients that connect via TCP to SwisTrack.
class ComponentOutputDataAssociationTraining: public Component {

public:
	//! Constructor.
	ComponentOutputDataAssociationTraining(SwisTrackCore *stc);
	//! Destructor.
	~ComponentOutputDataAssociationTraining();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {
		return new ComponentOutputDataAssociationTraining(mCore);
	}

private:
	wxString mFileName;	
};

#endif
