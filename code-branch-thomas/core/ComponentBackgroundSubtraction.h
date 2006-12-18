#ifndef HEADER_ComponentBackgroundSubtraction
#define HEADER_ComponentBackgroundSubtraction

#include "cv.h"
#include "Component.h"

class ComponentBackgroundSubtraction: public Component {

public:
	ComponentBackgroundSubtraction(SwisTrackCore *stc);
	~ComponentBackgroundSubtraction();

	// Overwritten Component methods
	bool Start();
	bool Step();
	bool Stop();

private:
	IplImage *mBackgroundImage;
	double mBackgroundImageMean;
};

#endif

