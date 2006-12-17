#ifndef HEADER_ComponentConvertToGray
#define HEADER_ComponentConvertToGray

#include "cv.h"
#include "Component.h"

class ComponentConvertToGray: public Component {

public:
	ComponentConvertToGray(SwisTrackCore *stc);
	~ComponentConvertToGray();

	// Overwritten Component methods
	bool Start();
	bool Step();
	bool Stop();

private:
};

#endif

