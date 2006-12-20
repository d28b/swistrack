#ifndef HEADER_ComponentBackgroundSubtractionGray
#define HEADER_ComponentBackgroundSubtractionGray

#include "cv.h"
#include "Component.h"

class ComponentBackgroundSubtractionGray: public Component {

public:
	ComponentBackgroundSubtractionGray(SwisTrackCore *stc, const std::string &displayname);
	~ComponentBackgroundSubtractionGray();

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

