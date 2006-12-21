#ifndef HEADER_ComponentInputCameraUSB
#define HEADER_ComponentInputCameraUSB

#include "cv.h"
#include "highgui.h"
#include "Component.h"

class ComponentInputCameraUSB: public Component {

public:
	ComponentInputCameraUSB(SwisTrackCore *stc);
	~ComponentInputCameraUSB();

	// Overwritten Component methods
	void OnStart();
	void OnStep();
	void OnStepCleanup();
	void OnStop();

	double GetFPS();

private:
	//! Pointer to the capture object.
	CvCapture* mCapture;
	//! The last acquired image.
	IplImage* mLastImage;
};

#endif

