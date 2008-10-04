#ifndef HEADER_ComponentOutputMarkFrameManual
#define HEADER_ComponentOutputMarkFrameManual

#include "Component.h"

//! A component that allows to manually marks frames. The markers are written to the output (which is usually send via TCP to clients). The actual image is left untouched.
class ComponentOutputMarkFrameManual: public Component {

public:
	//! Constructor.
	ComponentOutputMarkFrameManual(SwisTrackCore *stc);
	//! Destructor.
	~ComponentOutputMarkFrameManual();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {
		return new ComponentOutputMarkFrameManual(mCore);
	}

private:
	wxString mMarkerName;
};

#endif

