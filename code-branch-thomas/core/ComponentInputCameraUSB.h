#ifndef HEADER_ComponentInputCameraUSB
#define HEADER_ComponentInputCameraUSB

#include "cv.h"
#include "highgui.h"
#include "Component.h"
#include "DisplayImageStandard.h"

//! An input component that interfaces a USB camera using the CV library.
class ComponentInputCameraUSB: public Component {

public:
	//! Constructor.
	ComponentInputCameraUSB(SwisTrackCore *stc);
	//! Destructor.
	~ComponentInputCameraUSB();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {return new ComponentInputCameraUSB(mCore);}

	double GetFPS();

private:
	CvCapture* mCapture;		//!< Pointer to the capture object.
	IplImage* mOutputImage;	//!< The last acquired image.

	DisplayImageStandard mDisplayImageOutput;	//!< The DisplayImage showing the last acquired image.
};

#endif

