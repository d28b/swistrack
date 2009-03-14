#ifndef HEADER_ComponentMinCostFlowTracking
#define HEADER_ComponentMinCostFlowTracking

#include <cv.h>
#include "Component.h"
#include "DataStructureParticles.h"


#include "MinCostFlow.h"


//! A basic tracking component that takes pre-filtered particales and
// adds them to the nearest track.
class ComponentMinCostFlowTracking: public Component {

public:
	//! Constructor.
	ComponentMinCostFlowTracking(SwisTrackCore *stc);
	//! Destructor.
	~ComponentMinCostFlowTracking();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {
		return new ComponentMinCostFlowTracking(mCore);
	}

private:
	DataStructureTracks::tTrackMap mTracks;
	Display mDisplayOutput;	
	

};

#endif

