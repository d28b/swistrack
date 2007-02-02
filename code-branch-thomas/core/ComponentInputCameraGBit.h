#ifndef HEADER_ComponentInputCameraGBit
#define HEADER_ComponentInputCameraGBit

#include "Component.h"
#include "DisplayImageStandard.h"

//! An input component for GBit cameras.
class ComponentInputCameraGBit: public Component {

public:
	//! Constructor.
	ComponentInputCameraGBit(SwisTrackCore *stc);
	//! Destructor.
	~ComponentInputCameraGBit();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {return new ComponentInputCameraGBit(mCore);}

private:
	DisplayImageStandard mDisplayImageOutput;	//!< The DisplayImage showing the last acquired image.

};

#endif

