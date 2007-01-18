#ifndef HEADER_ComponentInputCameraUSB
#define HEADER_ComponentInputCameraUSB

#include "cv.h"
#include "highgui.h"
#include "Component.h"
#include "DisplayImageStandard.h"

class ComponentInputCameraUSB: public Component {

public:
	ComponentInputCameraUSB(SwisTrackCore *stc);
	~ComponentInputCameraUSB();

	// Overwritten Component methods
	void OnStart();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {return new ComponentInputCameraUSB(mCore);}

	double GetFPS();

private:
	//! Pointer to the capture object.
	CvCapture* mCapture;
	//! The last acquired image.
	IplImage* mCurrentImage;

	//! The DisplayImage showing the last acquired image.
	DisplayImageStandard mDisplayImageOutput;
};

#endif

