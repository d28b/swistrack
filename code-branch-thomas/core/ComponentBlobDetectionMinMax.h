#ifndef HEADER_ComponentBlobDetectionMinMax
#define HEADER_ComponentBlobDetectionMinMax

#include "cv.h"
#include "Component.h"
#include "DataStructureParticles.h"

class ComponentBlobDetectionMinMax: public Component {

public:
	ComponentBlobDetectionMinMax(SwisTrackCore *stc);
	~ComponentBlobDetectionMinMax();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {return new ComponentBlobDetectionMinMax(mCore);}

private:
	double mMinArea;
	double mMaxArea;
	int mMaxNumber;

	int mFirstDilate;
	int mFirstErode;
	int mSecondDilate;

	DataStructureParticles::tParticleVector mParticles;

	double GetContourCompactness(const void* contour);
};

#endif

