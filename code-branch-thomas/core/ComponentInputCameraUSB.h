#ifndef HEADER_ComponentInputCameraUSB
#define HEADER_ComponentInputCameraUSB

#include "cv.h"
#include "Component.h"

class ComponentInputCameraUSB: public Component {

public:
	ComponentInputCameraUSB(SwisTrackCore *stc);
	~ComponentInputCameraUSB();

	// Overwritten Component methods
	bool Start();
	bool Step();
	bool StepCleanup();
	bool Stop();

private:
	//! Pointer to the capture object.
	CvCapture* mCapture;
	//! The last acquired image.
	IplImage* mLastImage;
};

#endif

