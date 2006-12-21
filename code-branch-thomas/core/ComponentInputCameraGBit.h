#ifndef HEADER_ComponentInputCameraGBit
#define HEADER_ComponentInputCameraGBit

#include "Component.h"

class ComponentInputCameraGBit: public Component {

public:
	ComponentInputCameraGBit(SwisTrackCore *stc);
	~ComponentInputCameraGBit();

	// Overwritten Component methods
	void OnStart();
	void OnStep();
	void OnStepCleanup();
	void OnStop();

private:

};

#endif

