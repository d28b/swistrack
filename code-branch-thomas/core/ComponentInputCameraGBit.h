#ifndef HEADER_ComponentInputCameraGBit
#define HEADER_ComponentInputCameraGBit

#include "Component.h"

class ComponentInputCameraGBit: public Component {

public:
	ComponentInputCameraGBit(SwisTrackCore *stc, const std::string &displayname);
	~ComponentInputCameraGBit();

	// Overwritten Component methods
	bool Start();
	bool Step();
	bool StepCleanup();
	bool Stop();

private:

};

#endif

