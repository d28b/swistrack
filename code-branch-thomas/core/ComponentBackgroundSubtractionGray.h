#ifndef HEADER_ComponentBackgroundSubtractionGray
#define HEADER_ComponentBackgroundSubtractionGray

#include "cv.h"
#include "Component.h"

class ComponentBackgroundSubtractionGray: public Component {

public:
	ComponentBackgroundSubtractionGray(SwisTrackCore *stc);
	~ComponentBackgroundSubtractionGray();

	// Overwritten Component methods
	void OnStart();
	void OnStep();
	void OnStepCleanup();
	void OnStop();

private:
	IplImage *mBackgroundImage;
	double mBackgroundImageMean;
	bool mCorrectMean;
};

#endif

