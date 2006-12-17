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
	//! Pointer to the capture object.
	CvCapture* mCapture;
	//! The last acquired image.
	IplImage* mLastImage;
};

#endif

