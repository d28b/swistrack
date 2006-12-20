#ifndef HEADER_ComponentParticleFilter
#define HEADER_ComponentParticleFilter

#include "cv.h"
#include "Component.h"

class ComponentParticleFilter: public Component {

public:
	ComponentParticleFilter(SwisTrackCore *stc, const std::string &displayname);
	~ComponentParticleFilter();

	// Overwritten Component methods
	bool Start();
	bool Step();
	bool StepCleanup();
	bool Stop();

private:
	IplImage *mBackgroundImage;
	double mBackgroundImageMean;
};

#endif

