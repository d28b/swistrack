#ifndef HEADER_ComponentInputCameraGBit
#define HEADER_ComponentInputCameraGBit

#include "Component.h"

class ComponentInputCameraGBit: public Component {

public:
	ComponentInputCameraGBit(SwisTrackCore *stc);
	~ComponentInputCameraGBit();

	// Overwritten Component methods
	bool Start();
	bool Step();
	bool Stop();

private:

};

#endif

