#ifndef HEADER_ComponentThreshold
#define HEADER_ComponentThreshold

#include "cv.h"
#include "Component.h"

class ComponentThreshold: public Component {

public:
	ComponentThreshold(SwisTrackCore *stc);
	~ComponentThreshold();

	// Overwritten Component methods
	bool Start();
	bool Step();
	bool Stop();

private:
	IplImage *mLastImage;
};

#endif

