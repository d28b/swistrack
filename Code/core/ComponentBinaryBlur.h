#ifndef HEADER_ComponentBinaryBlur
#define HEADER_ComponentBinaryBlur

#include <opencv2/core.hpp>
#include "Component.h"

//! A component that blurs binary images.
class ComponentBinaryBlur: public Component {

public:
	//! Constructor.
	ComponentBinaryBlur(SwisTrackCore * stc);
	//! Destructor.
	~ComponentBinaryBlur();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();

	Component * Create() {
		return new ComponentBinaryBlur(mCore);
	}

private:
	int mSize;

	Display mDisplayOutput;
};

#endif

