#ifndef HEADER_ComponentInputCameraGBit
#define HEADER_ComponentInputCameraGBit

#include "Component.h"

class ComponentInputCameraGBit: public Component {

public:
	ComponentInputCameraGBit(xmlpp::Element* cfgRoot);
	~ComponentInputCameraGBit();

	// Overwritten Component methods
	void SetParameters();
	void Step();

private:

};

#endif

