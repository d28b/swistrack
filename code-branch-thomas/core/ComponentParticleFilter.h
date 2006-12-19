#ifndef HEADER_ComponentParticleFilter
#define HEADER_ComponentParticleFilter

#include "cv.h"
#include "Component.h"

class ComponentParticleFilter: public Component {

public:
	ComponentParticleFilter(SwisTrackCore *stc);
	~ComponentParticleFilter();

	// Overwritten Component methods
	bool Start();
	bool Step();
	bool StepCleanup();
	bool Stop();

private:
	double mMinArea;
	double mMaxArea;
	int mMaxNumber;
};

#endif

